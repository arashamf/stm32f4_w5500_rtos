#ifndef __NET_H__
#define __NET_H__

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#include "stm32f4xx_hal.h"

//--------------------------------------------------

//--------------------------------------------------
void w5500_driver_init (void);
void socket_init (void);

#ifdef __cplusplus
}
#endif

#endif /* __NET_H__ */
