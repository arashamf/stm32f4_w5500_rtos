#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "driver_w5500.h"

//--------------------------------------------------
#define 	MAX_SIZE_SOCKET 	    32
#define     UDP_PORT                1111
#define     RECV_INT_MASK           0x04  
#define     ALLINT_OFF(x)           setSn_IMR(x, 0)
#define     INT_RECV_ON(x)          setSn_IMR(x, RECV_INT_MASK)
//--------------------------------------------------
enum STATE
{
    ERR_MSG   =     -1,
    NO_MSG    =     0,
    GET_MSG   =     1,
    PUT_MSG   =     2
};


//--------------------------------------------------
typedef struct {
uint8_t input_IP[4];
uint16_t input_port;
uint16_t my_port;
volatile uint8_t cur_socket;  //активный сокет
} udp_prop_ptr;

extern udp_prop_ptr udpprop;
extern uint8_t rx_buff[];
//--------------------------------------------------
void w5500_driver_init (void);
void socket_init (void);
//--------------------------------------------------
int8_t send_UDPmsg (udp_prop_ptr * handle, char * buf, uint8_t len);
int8_t get_UDPmsg (udp_prop_ptr * handle, uint8_t *buff, uint16_t size);
void check_UDPnet(void);

#ifdef __cplusplus
}
#endif

#endif /* __NET_H__ */
