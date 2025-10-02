#ifndef NET_H_
#define NET_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "w5500.h"

//--------------------------------------------------
#define IP_ADDR {192,168,111,197}
#define IP_GATE {192,168,111,11}
#define IP_MASK {255,255,255,0}
#define LOCAL_PORT 80
//--------------------------------------------------
#define be16toword(a) ((((a)>>8)&0xff)|(((a)<<8)&0xff00))
//--------------------------------------------------
void net_poll(void);
void net_ini(void);

#ifdef __cplusplus
}
#endif

#endif /* NET_H_ */