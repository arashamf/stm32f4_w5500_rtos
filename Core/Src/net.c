#include "net.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "socket.h"
#include <string.h>

/* FreeRTOS includes. */
#include <FreeRTOS.h>
#include <task.h>
//----------------------------------------------------------------------------------------------------------//
#ifdef DEBUG_MODE 
//extern char dbg_buf[];
#endif

uint8_t rx_buff[512]; //Receive data 
udp_prop_ptr udpprop = {0}; // структура для свойств соединения сокета
TaskHandle_t UDPTask_Handler=NULL;

//------------------------------------------------------
wiz_NetInfo net_info = {
  .mac    =  MAC_ADDR,
  .ip     =  IP_ADDR ,
  .gw     =  GATE_ADDR  ,
  .sn     =  NET_MASK,
  .dhcp = NETINFO_STATIC
};

//----------------------------------------------------------------------------------------------------------//
extern void W5500_ReadBuff(uint8_t* buff, uint16_t len);
extern void W5500_WriteBuff(uint8_t* buff, uint16_t len);
extern uint8_t W5500_ReadByte(void);
extern void W5500_WriteByte(uint8_t byte);
extern void W5500_Select (void);
extern void W5500_Unselect (void);

void udp_task(void *pvParameters);
//----------------------------------------------------------------------------------------------------------//
void w5500_driver_init (void)
{
  hard_reset (50); //хард ресет w5500
  reg_wizchip_cs_cbfunc( W5500_Select, W5500_Unselect );
  reg_wizchip_spi_cbfunc( W5500_ReadByte , W5500_WriteByte );
  reg_wizchip_spiburst_cbfunc( W5500_ReadBuff, W5500_WriteBuff );

  /*uint8_t W5500_fifo_size[2][8] = { {2, 2, 2, 2, 2, 2, 2, 2},   //каждый элемент массива - размер соотвествующего буффера приёма (всего 8 буфферов)
                                    {2, 2, 2, 2, 2, 2, 2, 2} }; //буфферы Rx, общий размер всех буфферов не более 16 кБайт
  if ( ctlwizchip( CW_INIT_WIZCHIP , (void*)W5500_fifo_size ) ==  -1)
  {
      #ifdef DEBUG_MODE 
      dbg_putStr ("Create_buffers_error\r\n");
      #endif
  }*/
  setSn_RXBUF_SIZE(udpprop.cur_socket, MAX_SIZE_SOCKET/4); //выделение памяти буфер приёма сокета
  setSn_TXBUF_SIZE(udpprop.cur_socket, MAX_SIZE_SOCKET/4); //выделение памяти буфер передачи сокета
  setSIMR(1<<udpprop.cur_socket); //Set SIMR register. Each bit of SIMR corresponds to each bit of SIR. When a bit of SIMR is ‘1’ and the corresponding bit of SIR is ‘1’, Interrupt will be issued.
  INT_RECV_ON (udpprop.cur_socket);
  setINTLEVEL(99);

  wizchip_setnetinfo(&net_info); //установка сетевых настроек
  uint8_t phycfgr_stat;
  do
  {
    if( ctlwizchip( CW_GET_PHYLINK, (void*) &phycfgr_stat ) == -1 ) //check connection. Get ref PHYCFGR register
    {
        #ifdef DEBUG_MODE 
        dbg_putStr ("NO_LINK\r\n");
        #endif
    }
  }
  while(phycfgr_stat == PHY_LINK_OFF);  
}

//----------------------------------------------------------------------------------------------------------//
void socket_init (void)
{
    udpprop.my_port = UDP_PORT;
    uint8_t io_mode = SOCK_IO_NONBLOCK; // Set non-block io mode 
    //uint8_t io_mode = SOCK_IO_BLOCK;

    udpprop.cur_socket = DEF_SOCKET;
    int8_t status;
    //Control socket. Control IO mode, Interrupt & Mask of socket and get the socket buffer information
    status = ctlsocket(udpprop.cur_socket, CS_SET_IOMODE, &io_mode);
    { 
      #ifdef DEBUG_MODE 
      sprintf(dbg_buf, "ctlsock_type = %d\r\n", status); //Wait for status set 
      dbg_putStr (dbg_buf);  
      #endif
    }   

    // Open TCP socket. Initializes the socket with 'sn' passed as parameter and open
   // uint8_t retval = socket(udpprop.cur_socket, Sn_MR_TCP, port, 0);  
    uint8_t retval = socket(udpprop.cur_socket,Sn_MR_UDP,udpprop.my_port,io_mode);
    if ( retval != udpprop.cur_socket ) { 
      #ifdef DEBUG_MODE 
      dbg_putStr ("open_socket_error\r\n"); 
      #endif  
    }  

    do  {
      #ifdef DEBUG_MODE 
      sprintf(dbg_buf, "open_socket_status_Sn_SR = %x\r\n", getSn_SR(udpprop.cur_socket) ); //Wait for status set 
      dbg_putStr (dbg_buf);
      HAL_Delay(50);
      #endif
    }
    #ifdef UDP_MODE
    while ( getSn_SR(udpprop.cur_socket) != SOCK_UDP ); //Socket_register_access_function. Get Sn_SR register
    #endif 
    #ifdef TCP_MODE
    while ( getSn_SR(udpprop.cur_socket) != SOCK_INIT ); //Socket_register_access_function. Get Sn_SR register


    retval = listen(udpprop.cur_socket); // Listen to a connection request from a client
    if ( retval != SOCK_OK ) 
    { 
      #ifdef DEBUG_MODE 
      dbg_putStr ("Socket is not initialized or closed unexpectedly\r\n"); 
      #endif
    } 

    do
    {
      #ifdef DEBUG_MODE 
      sprintf(dbg_buf, "listen_socket_status_Sn_SR = %x\r\n", getSn_SR(udpprop.cur_socket) ); //Wait for status set 
      dbg_putStr (dbg_buf);
      HAL_Delay(10);
      #endif
    }
    while ( getSn_SR(udpprop.cur_socket) != SOCK_LISTEN ) ; ///Socket_register_access_function. Get Sn_SR register

    #ifdef DEBUG_MODE 
    dbg_putStr("Listening...\r\n");
    #endif

    #endif

    xTaskCreate((TaskFunction_t )udp_task, (const char*)"com_task", 128, (void* )NULL, (UBaseType_t)2,	(TaskHandle_t*)&UDPTask_Handler);
}

//--------------------------------------------------
int8_t send_UDPmsg (udp_prop_ptr * handle, char * buf, uint8_t len)
{
  int8_t conn_status = NO_MSG;
  return (conn_status = sendto(udpprop.cur_socket, (uint8_t *)buf, len, handle->input_IP, handle->input_port));
}


//--------------------------------------------------
int8_t get_UDPmsg (udp_prop_ptr * handle, uint8_t *buff, uint16_t size)
{
  int8_t conn_status = NO_MSG;
  uint32_t msg_len;

  if ( getSn_SR(handle->cur_socket) != SOCK_UDP ) 
  {
    #ifdef DEBUG_MODE 
    dbg_putStr ("udp_socket error\r\n"); 
    #endif
    return (conn_status = ERR_MSG);
  }    

  if ((msg_len = recvfrom(handle->cur_socket, buff, size, handle->input_IP, &handle->input_port)) > 0)
  {
      #ifdef  DEBUG_MODE 
      buff[msg_len] = '\0';
      snprintf(dbg_buf, dbg_buf_size, "remote_IP:port=%03d.%03d.%03d.%03d:%05d\r\nmsg=%s\r\n", handle->input_IP[0],  handle->input_IP[1], 
      handle->input_IP[2],  handle->input_IP[3], handle->input_port, buff);
      dbg_putStr (dbg_buf);
      #endif
      conn_status = GET_MSG;    
  }
  return (conn_status);
}

//--------------------------------------------------
void check_UDPnet(void)
{
  if ((get_UDPmsg (&udpprop, rx_buff, sizeof (rx_buff))) == GET_MSG)
  { send_UDPmsg (&udpprop, "answer_OK", 9); }
}

//---------------------------------------------------------------------------//
void udp_task(void *pvParameters)
{

    for(;;)
    {
        if ((get_UDPmsg (&udpprop, rx_buff, sizeof (rx_buff))) == GET_MSG)
        {  
          ALLINT_OFF(udpprop.cur_socket);
          setSn_IR(0, 0);
          send_UDPmsg (&udpprop, "answer_OK", 9); 
          #ifdef DEBUG_MODE 
          sprintf (dbg_buf, "SIR=%d, SIMR=%d, S0_IR=%d, S0_IMR=%d\r\n", getSIR(),  getSIMR(), getSn_IR(0), getSn_IMR(0));
          dbg_putStr(dbg_buf);
          #endif
          INT_RECV_ON(udpprop.cur_socket);
        }
        vTaskDelay(50);
    }
}