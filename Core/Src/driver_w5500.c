#include "driver_w5500.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>

//-------------------------------------------------------------------------------------------------------------//
#ifdef DEBUG_MODE 
extern char dbg_buf[];
#endif

char tmpbuf[30]; //массив для временного буфера
uint8_t sect[515]; //глобальный массив для сектора, который будет привязыватся к определённой структуре
extern tcp_prop_ptr tcpprop; // структуру для свойств соединения сокета
//configute Net
uint8_t macaddr[6]=MAC_ADDR;
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];

//-------------------------------------------------------------------------------------------------------------//
void w5500_writeReg(uint8_t op, uint16_t addres, uint8_t data);
uint8_t w5500_readReg(uint8_t op, uint16_t addres);
void w5500_readBuf(data_sect_ptr *datasect, uint16_t len);
void w5500_readSockBuf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len);
uint16_t GetSizeRX(uint8_t sock_num);

//-------------------------------------------запись в регистр W5500-------------------------------------------//
void w5500_writeReg(uint8_t op, uint16_t addres, uint8_t data)
{
    uint8_t buf[] = {addres >> 8, addres, op|(RWB_WRITE<<2), data}; //RWB_WRITE - бит записи
    W5500_Select ();
    W5500_WriteBuff (buf, 4);
    W5500_Unselect ();
}

//-------------------------------------------чтение регистра W5500-------------------------------------------//
uint8_t w5500_readReg(uint8_t op, uint16_t addres)
{
    uint8_t data;
    uint8_t wbuf[] = {addres >> 8, addres, op, 0x0};
    uint8_t rbuf[4] = {0};
    W5500_write_read_data (wbuf, rbuf, 4);
    data = rbuf[3];
    return data;
}

//-------------------------------------------------------------------------------------------------------------//
void w5500_readBuf(data_sect_ptr *datasect, uint16_t len)
{
    W5500_write_data  ((uint8_t*) datasect, 3);
    W5500_read_data  ((uint8_t*) datasect, len);
}

//--------------------------------чтение байта из буффера определённого сокета--------------------------------//
uint8_t w5500_readSockBufByte(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode, bt;
  opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM1;
  bt = w5500_readReg(opcode, point);
  return bt;
}

//------------------------------------чтение буффера определённого сокета------------------------------------//
void w5500_readSockBuf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_readBuf(datasect,len);
}

//------------------------------------инициализация сокета------------------------------------//
void OpenSocket(uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, NUMB_MR, mode);
  w5500_writeReg(opcode, NUMB_CR, 0x01);
}

//--------------------------ожидание окончания инициализации сокета--------------------------//
void SocketInitWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, NUMB_SR)==SOCK_INIT)
    {   break;  }
  }
}

//-------------------------------------включение режима прослушки сокета--------------------------------------//
void ListenSocket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, NUMB_CR, 0x02); //перевод сокета в состояние "LISTEN"
}

//---------------------------------ожидание установки режима прослушки сокета---------------------------------//
void SocketListenWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, NUMB_SR)==SOCK_LISTEN) //функция ожидает, когда состояние LISTEN установится
    {   break;  }
  }
}

//-------------------------------------------------------------------------------------------------------------//
void DisconnectSocket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1; //only in TCP mode. Отлючение сокета
  w5500_writeReg(opcode, NUMB_CR, 0x08); //DISCON
}


//-------------------------------------------------------------------------------------------------------------//
void SocketClosedWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, NUMB_SR)==SOCK_CLOSED) //функция ожидает, когда состояние SOCK_CLOSED установится
    {   break;  }
  }
}

//-----------------------------------------получение статуса сокета-----------------------------------------//
uint8_t GetSocketStatus(uint8_t sock_num)
{
  uint8_t dt;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  dt = w5500_readReg(opcode, NUMB_SR);
  return dt;
}

//-----------------------------------------------------------------------------------------------------------//
void RecvSocket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, NUMB_CR, 0x40); //Функция RECV завершает обработку полученных данных в буфере приёма сокета n, используя регистр указателя чтения приёма (Sn_RX_RD)
}

//-----------------------------------------------------------------------------------------------------------//
void SendSocket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, NUMB_CR, 0x20); //Команда SEND передаёт все данные из буфера Socket n TX buffer. 
}

//----------------------------ф-я определения размера принятых данных в TCP-пакете----------------------------//
uint16_t GetSizeRX(uint8_t sock_num)
{
  uint16_t len;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  len = (w5500_readReg(opcode,Sn_RX_RSR0)<<8|w5500_readReg(opcode,Sn_RX_RSR1)); //Sn_RX_RSR указывает размер данных, полученных и сохранённых в буфере приёма сокета n
  return len;
}

//-----------------------------------------------------------------------------------------------------------//
uint16_t GetReadPointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_readReg(opcode,Sn_RX_RD0)<<8|w5500_readReg(opcode,Sn_RX_RD1)); //Socket n RX Read Data Pointer Register
  return point;
}

//-----------------------------------------------------------------------------------------------------------//
uint16_t GetWritePointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_readReg(opcode,Sn_TX_WR0)<<8|w5500_readReg(opcode,Sn_TX_WR1)); //Read the starting address for saving the transmitting data
  return point;
}

//-----------------------------------------------------------------------------------------------------------//
void SetWritePointer(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_TX_WR0, point>>8); //Save the transmitting data from the starting address of Socket n TX buffer
  w5500_writeReg(opcode, Sn_TX_WR1, (uint8_t)point);
}

//-----------------------------------------------------------------------------------------------------------//
void w5500_ini(wiz_NetInfo * handleNet, uint16_t local_port)
{
  uint8_t dtt=0;
  uint8_t opcode=0;
  hard_reset (50);    //Hard Reset
	opcode = (BSB_COMMON<<3)|OM_FDM1; //Soft Reset
	w5500_writeReg(opcode, MODEREG, 0x80);
	HAL_Delay(100);
	//Configute Net
	w5500_writeReg(opcode, SHAR0,handleNet->mac[0]);
	w5500_writeReg(opcode, SHAR1,handleNet->mac[1]);
	w5500_writeReg(opcode, SHAR2,handleNet->mac[2]);
	w5500_writeReg(opcode, SHAR3,handleNet->mac[3]);
	w5500_writeReg(opcode, SHAR4,handleNet->mac[4]);
	w5500_writeReg(opcode, SHAR5,handleNet->mac[5]);
	w5500_writeReg(opcode, GWR0,handleNet->gw[0]);
	w5500_writeReg(opcode, GWR1,handleNet->gw[1]);
	w5500_writeReg(opcode, GWR2,handleNet->gw[2]);
	w5500_writeReg(opcode, GWR3,handleNet->gw[3]);
	w5500_writeReg(opcode, SUBR0,handleNet->sn[0]);
	w5500_writeReg(opcode, SUBR1,handleNet->sn[1]);
	w5500_writeReg(opcode, SUBR2,handleNet->sn[2]);
	w5500_writeReg(opcode, SUBR3,handleNet->sn[3]);
	w5500_writeReg(opcode, SIPR0,handleNet->ip[0]);
	w5500_writeReg(opcode, SIPR1,handleNet->ip[1]);
	w5500_writeReg(opcode, SIPR2,handleNet->ip[2]);
	w5500_writeReg(opcode, SIPR3,handleNet->ip[3]);
	//Настройка сокета 0
	opcode = (BSB_S0<<3)|OM_FDM1;
	w5500_writeReg(opcode, Sn_PORT0, local_port>>8);
	w5500_writeReg(opcode, Sn_PORT1, local_port);
	tcpprop.cur_sock = 0;

	OpenSocket(0,Mode_TCP); //Открытие сокета 0
	SocketInitWait(0); //ожидание окончания инициализации сокета

	ListenSocket(0); 	//Начало прослушки сокета
	SocketListenWait(0);
  HAL_Delay(500);
  
  opcode = (BSB_S0<<3)|OM_FDM1; //Просмотр статусов
  dtt = w5500_readReg(opcode, NUMB_SR);

  #if DEBUG_MODE == 1
  sprintf(dbg_buf,"First Status Sn0: 0x%02X\r\n",dtt);
  dbg_putStr (dbg_buf);
  #endif
}

//-----------------------------------------------------------------------------------------------------------//
void w5500_packetReceive(void)
{
    uint16_t point;
    uint16_t len;

/*	if(GetSocketStatus(tcpprop.cur_sock)==SOCK_ESTABLISHED)     
  {
		if(httpsockprop[tcpprop.cur_sock].data_stat == DATA_COMPLETED)      
    {
			len = GetSizeRX(0); //len - size of received data
      #ifdef DEBUG_MODE 
      sprintf(dbg_buf,"len_rx_buf:0x%04X\r\n", len);
      dbg_putStr (dbg_buf);
      #endif

      DisconnectSocket(tcpprop.cur_sock);   //инициализация разъединия с сокетом
      SocketClosedWait(tcpprop.cur_sock);   //ожидание окончания разъединия   
      OpenSocket(tcpprop.cur_sock,Mode_TCP);
      SocketInitWait(tcpprop.cur_sock);       //Ждём инициализации сокета (статус SOCK_INIT)
      ListenSocket(tcpprop.cur_sock);         //Продолжаем слушать сокет
      SocketListenWait(tcpprop.cur_sock);

			if(!len) return; //Если пришел пустой пакет, то уходим из функции
      if (strncmp(tmpbuf,"GET /", 5) == 0)    
      {
        dbg_putStr ("HTTP\r\n");
				httpsockprop[tcpprop.cur_sock].prt_tp = PRT_TCP_HTTP;
				http_request();
			}
			//здесь обмениваемся информацией: на запрос документа от клиента отправляем ему запрошенный документ
			//указатель на начало чтения приёмного буфера
			point = GetReadPointer(tcpprop.cur_sock);
			sprintf(str1,   "Sn_RX_RD:0x%04X\r\n",  point);
			HAL_UART_Transmit(&huart2,  (uint8_t*)str1, strlen(str1),   0x1000);
			w5500_readSockBuf(tcpprop.cur_sock, point, (uint8_t*)tmpbuf, 5);
	
		}
		else    
    {
      if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_MIDDLE)   
      {
        if(httpsockprop[tcpprop.cur_sock].prt_tp == PRT_TCP_HTTP)
        {   
          //tcp_send_http_middle(); 
        }
      }
      else    
      {
        if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_LAST)     
        {
          if(httpsockprop[tcpprop.cur_sock].prt_tp == PRT_TCP_HTTP)       
          {
				    //tcp_send_http_last();
            DisconnectSocket(tcpprop.cur_sock);     //Разъединяемся
            SocketClosedWait(tcpprop.cur_sock);
            OpenSocket(tcpprop.cur_sock,Mode_TCP);
            SocketInitWait(tcpprop.cur_sock);       //Ждём инициализации сокета (статус SOCK_INIT)
            ListenSocket(tcpprop.cur_sock);         //Продолжаем слушать сокет
            SocketListenWait(tcpprop.cur_sock);
          }
        }
      }     
    }
  }*/
}

//-----------------------------------------------------------------------------------------------------------//
