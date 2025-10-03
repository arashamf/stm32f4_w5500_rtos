#include "net.h"
#include "spi.h"
#include "gpio.h"
#include "usart.h"
#include "socket.h"
#include "driver_w5500.h"
//#include "w5500.h"

//----------------------------------------------------------------------------------------------------------//
#ifdef DEBUG_MODE 
extern char dbg_buf[];
#endif

//----------------------------------------------------------------------------------------------------------//
wiz_NetInfo net_info = {
  .mac    =  MAC_ADDR,
  .ip     =  IP_ADDR ,
  .gw     =  GATE_ADDR  ,
  .sn     =  NET_MASK,
  .dhcp = NETINFO_STATIC
};

tcp_prop_ptr tcpprop; // структура для свойств соединения сокета

extern void W5500_ReadBuff(uint8_t* buff, uint16_t len);
extern void W5500_WriteBuff(uint8_t* buff, uint16_t len);
extern uint8_t W5500_ReadByte(void);
extern void W5500_WriteByte(uint8_t byte);
extern void W5500_Select (void);
extern void W5500_Unselect (void);

//----------------------------------------------------------------------------------------------------------//
void w5500_driver_init (void)
{
  hard_reset (50); //хард ресет w5500
  reg_wizchip_cs_cbfunc( W5500_Select, W5500_Unselect );
  reg_wizchip_spi_cbfunc( W5500_ReadByte , W5500_WriteByte );
  reg_wizchip_spiburst_cbfunc( W5500_ReadBuff, W5500_WriteBuff );

  uint8_t W5500_fifo_size[2][8] = { {2, 2, 2, 2, 2, 2, 2, 2},
                                    {2, 2, 2, 2, 2, 2, 2, 2} };
  if ( ctlwizchip( CW_INIT_WIZCHIP , (void*)W5500_fifo_size ));

  wizchip_setnetinfo(&net_info);

  uint8_t phycfgr_stat;
  do
  {
    if( ctlwizchip( CW_GET_PHYLINK, (void*) &phycfgr_stat ) == -1 ) {}//check  
  }
  while(phycfgr_stat == PHY_LINK_OFF);  
}

//----------------------------------------------------------------------------------------------------------//
void socket_init (void)
{
    uint8_t io_mode = SOCK_IO_NONBLOCK; // Set non-block io mode 
    uint32_t port = DEF_PORT;
    tcpprop.cur_sock = DEF_SOCKET;

    if ( ctlsocket(tcpprop.cur_sock, CS_SET_IOMODE, &io_mode ) == SOCKERR_ARG )
    { 
        #ifdef DEBUG_MODE 
        dbg_putStr ("socket_error\r\n"); 
        #endif
    }   

    uint8_t retval = socket(tcpprop.cur_sock, Sn_MR_TCP, port, 0);  // Open TCP socket 
    if ( retval != tcpprop.cur_sock )
    { 
        #ifdef DEBUG_MODE 
        dbg_putStr ("socket_error\r\n"); 
        #endif
    }  

    do
    {
        #ifdef DEBUG_MODE 
        sprintf(dbg_buf, "_socket_status_Sn_SR = %x\r\n", getSn_SR(tcpprop.cur_sock) ); //Wait for status set 
        dbg_putStr (dbg_buf);
        HAL_Delay(50);
        #endif
    }
    while ( getSn_SR(tcpprop.cur_sock) != SOCK_INIT );

    retval = listen(tcpprop.cur_sock); //listen TCP socket 
    if ( retval != SOCK_OK ) 
    { 
        #ifdef DEBUG_MODE 
        dbg_putStr ("socket_error\r\n"); 
        #endif
    } 

    do
    {
        #ifdef DEBUG_MODE 
        sprintf(dbg_buf, "_socket_status_Sn_SR = %x\r\n", getSn_SR(tcpprop.cur_sock) ); //Wait for status set 
        dbg_putStr (dbg_buf);
        HAL_Delay(10);
        #endif
    }
    while ( getSn_SR(tcpprop.cur_sock) != SOCK_LISTEN ) ; //Wait for status set 

    #ifdef DEBUG_MODE 
    dbg_putStr("Listening...\r\n");
    #endif
}
