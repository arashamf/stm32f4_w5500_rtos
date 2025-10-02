#include "w5500.h"
#include "httpd.h"
#include "gpio.h"
#include <string.h>
#include <stdio.h>

//-----------------------------------------------
extern SPI_HandleTypeDef hspi1;
//extern UART_HandleTypeDef huart2;
//extern char str1[60];
char tmpbuf[30];
uint8_t sect[515];
tcp_prop_ptr tcpprop;
extern http_sock_prop_ptr httpsockprop[2];

//-----------------------------------------------
uint8_t macaddr[6]=MAC_ADDR;
extern uint8_t ipaddr[4];
extern uint8_t ipgate[4];
extern uint8_t ipmask[4];
extern uint16_t local_port;

//-----------------------------------------------
void w5500_writeReg(uint8_t op, uint16_t addres, uint8_t data)
{
    uint8_t buf[] = {addres >> 8, addres, op|(RWB_WRITE<<2), data};
    W5500_CS(ON);
    HAL_SPI_Transmit(&hspi1, buf, 4, 0xFFFFFFFF);
    W5500_CS(OFF);
}

//-----------------------------------------------
void w5500_writeBuf(data_sect_ptr *datasect, uint16_t len)
{
    W5500_CS(ON);
    HAL_SPI_Transmit(&hspi1, (uint8_t*) datasect, len, 0xFFFFFFFF);
    W5500_CS(OFF);
}

//-----------------------------------------------
void w5500_writeSockBuf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_TX)<<3)|(RWB_WRITE<<2)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_writeBuf(datasect,len+3);//3 служебных байта
}

//-----------------------------------------------
uint8_t w5500_readReg(uint8_t op, uint16_t addres)
{
    uint8_t data;
    uint8_t wbuf[] = {addres >> 8, addres, op, 0x0};
    uint8_t rbuf[4] = {0};

    W5500_CS(ON);
    HAL_SPI_TransmitReceive(&hspi1, wbuf, rbuf, 4, 0xFFFFFFFF);
    W5500_CS(OFF);

    data = rbuf[3];
    return data;
}

//-----------------------------------------------
void w5500_readBuf(data_sect_ptr *datasect, uint16_t len)
{
    W5500_CS(ON);
    HAL_SPI_Transmit(&hspi1, (uint8_t*) datasect, 3, 0xFFFFFFFF);
    HAL_SPI_Receive(&hspi1, (uint8_t*) datasect, len, 0xFFFFFFFF);
    W5500_CS(OFF);
}

//-----------------------------------------------
uint8_t w5500_readSockBufByte(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode, bt;
  opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM1;
  bt = w5500_readReg(opcode, point);
  return bt;
}

//-----------------------------------------------
void w5500_readSockBuf(uint8_t sock_num, uint16_t point, uint8_t *buf, uint16_t len)
{
  data_sect_ptr *datasect = (void*)buf;
  datasect->opcode = (((sock_num<<2)|BSB_S0_RX)<<3)|OM_FDM0;
  datasect->addr = be16toword(point);
  w5500_readBuf(datasect,len);
}

//-----------------------------------------------
void OpenSocket(uint8_t sock_num, uint16_t mode)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_MR, mode);
  w5500_writeReg(opcode, Sn_CR, 0x01);
}

//-----------------------------------------------
void SocketInitWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, Sn_SR)==SOCK_INIT)
    {   break;  }
  }
}

//-----------------------------------------------
void ListenSocket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_CR, 0x02); //LISTEN SOCKET
}

//-----------------------------------------------
void SocketListenWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, Sn_SR)==SOCK_LISTEN)
    {   break;  }
  }
}

//-----------------------------------------------
void SocketClosedWait(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  while(1)
  {
    if(w5500_readReg(opcode, Sn_SR)==SOCK_CLOSED)
    {   break;  }
  }
}

//-----------------------------------------------
void DisconnectSocket(uint8_t sock_num)
{
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_CR, 0x08); //DISCON
}

//-----------------------------------------------
uint8_t GetSocketStatus(uint8_t sock_num)
{
  uint8_t dt;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  dt = w5500_readReg(opcode, Sn_SR);
  return dt;
}

//-----------------------------------------------
void RecvSocket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_CR, 0x40); //RECV SOCKET
}
//-----------------------------------------------
void SendSocket(uint8_t sock_num)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_CR, 0x20); //SEND SOCKET
}

//-----------------------------------------------
uint16_t GetSizeRX(uint8_t sock_num)
{
  uint16_t len;
  uint8_t opcode=0;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  len = (w5500_readReg(opcode,Sn_RX_RSR0)<<8|w5500_readReg(opcode,Sn_RX_RSR1));
  return len;
}

//-----------------------------------------------
uint16_t GetReadPointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_readReg(opcode,Sn_RX_RD0)<<8|w5500_readReg(opcode,Sn_RX_RD1));
  return point;
}

//-----------------------------------------------
uint16_t GetWritePointer(uint8_t sock_num)
{
  uint16_t point;
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  point = (w5500_readReg(opcode,Sn_TX_WR0)<<8|w5500_readReg(opcode,Sn_TX_WR1));
  return point;
}

//-----------------------------------------------
void SetWritePointer(uint8_t sock_num, uint16_t point)
{
  uint8_t opcode;
  opcode = (((sock_num<<2)|BSB_S0)<<3)|OM_FDM1;
  w5500_writeReg(opcode, Sn_TX_WR0, point>>8);
  w5500_writeReg(opcode, Sn_TX_WR1, (uint8_t)point);
}

//-----------------------------------------------
void w5500_ini(void)
{
  uint8_t dtt=0;
  uint8_t opcode=0;
  hard_reset (50);    //Hard Reset
	opcode = (BSB_COMMON<<3)|OM_FDM1; //Soft Reset
	w5500_writeReg(opcode, MR, 0x80);
	HAL_Delay(100);
	//Configute Net
	w5500_writeReg(opcode, SHAR0,macaddr[0]);
	w5500_writeReg(opcode, SHAR1,macaddr[1]);
	w5500_writeReg(opcode, SHAR2,macaddr[2]);
	w5500_writeReg(opcode, SHAR3,macaddr[3]);
	w5500_writeReg(opcode, SHAR4,macaddr[4]);
	w5500_writeReg(opcode, SHAR5,macaddr[5]);
	w5500_writeReg(opcode, GWR0,ipgate[0]);
	w5500_writeReg(opcode, GWR1,ipgate[1]);
	w5500_writeReg(opcode, GWR2,ipgate[2]);
	w5500_writeReg(opcode, GWR3,ipgate[3]);
	w5500_writeReg(opcode, SUBR0,ipmask[0]);
	w5500_writeReg(opcode, SUBR1,ipmask[1]);
	w5500_writeReg(opcode, SUBR2,ipmask[2]);
	w5500_writeReg(opcode, SUBR3,ipmask[3]);
	w5500_writeReg(opcode, SIPR0,ipaddr[0]);
	w5500_writeReg(opcode, SIPR1,ipaddr[1]);
	w5500_writeReg(opcode, SIPR2,ipaddr[2]);
	w5500_writeReg(opcode, SIPR3,ipaddr[3]);
	//Настройка сокета 0
	opcode = (BSB_S0<<3)|OM_FDM1;
	w5500_writeReg(opcode, Sn_PORT0,local_port>>8);
	w5500_writeReg(opcode, Sn_PORT1,local_port);
	
	tcpprop.cur_sock = 0;
	OpenSocket(0,Mode_TCP); //Открытие сокета 0
	SocketInitWait(0); //инициализация сокета
	ListenSocket(0); 	//Начало прослушки сокета
	SocketListenWait(0);
  HAL_Delay(500);
  
  opcode = (BSB_S0<<3)|OM_FDM1; //Просмотр статусов
  dtt = w5500_readReg(opcode, Sn_SR);

  #if DEBUG == 1
  sprintf(str1,"First Status Sn0: 0x%02X\r\n",dtt);
  HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);	
  #endif
}

//-----------------------------------------------
void w5500_packetReceive(void)
{
    uint16_t point;
    uint16_t len;

	if(GetSocketStatus(tcpprop.cur_sock)==SOCK_ESTABLISHED)     {
		/*if(httpsockprop[tcpprop.cur_sock].data_stat == DATA_COMPLETED)      {
			len = GetSizeRX(0); //Отобразим размер принятых данных
			sprintf(str1,   "len_rx_buf:0x%04X\r\n",    len);
			HAL_UART_Transmit(&huart2,  (uint8_t*)str1, strlen(str1),   0x1000);
			if(!len) return; //Если пришел пустой пакет, то уходим из функции
			//здесь обмениваемся информацией: на запрос документа от клиента отправляем ему запрошенный документ
			//указатель на начало чтения приёмного буфера
			point = GetReadPointer(tcpprop.cur_sock);
			sprintf(str1,   "Sn_RX_RD:0x%04X\r\n",  point);
			HAL_UART_Transmit(&huart2,  (uint8_t*)str1, strlen(str1),   0x1000);
			w5500_readSockBuf(tcpprop.cur_sock, point, (uint8_t*)tmpbuf, 5);
			if (strncmp(tmpbuf,"GET /", 5) == 0)    {
				HAL_UART_Transmit(  &huart2,  (uint8_t*)"HTTP\r\n", 6,  0x1000);
				httpsockprop[tcpprop.cur_sock].prt_tp = PRT_TCP_HTTP;
				//http_request();
			}
		}
		else    {
            if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_MIDDLE)   {
                if(httpsockprop[tcpprop.cur_sock].prt_tp == PRT_TCP_HTTP)
                {   
                    //tcp_send_http_middle(); 
                }
            }
            else    {
                if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_LAST)     {
                    if(httpsockprop[tcpprop.cur_sock].prt_tp == PRT_TCP_HTTP)       {
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
        }*/
  }
}
//-----------------------------------------------
