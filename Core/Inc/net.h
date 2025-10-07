#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "driver_w5500.h"

//--------------------------------------------------
#define 	MAX_SIZE_SOCKET 	32

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
uint8_t IP[4];
uint16_t port;
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
