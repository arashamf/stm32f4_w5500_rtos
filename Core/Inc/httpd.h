#ifndef HTTPD_H_
#define HTTPD_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include "main.h"
//--------------------------------------------------
typedef struct http_sock_prop {
	volatile uint8_t data_stat;//статус передачи данных
	volatile uint32_t data_size;//размер данных для передачи
	volatile uint16_t last_data_part_size;//размер последней части данных для передачи
	volatile uint16_t cnt_data_part;//общее количество частей данных для передачи
	volatile uint16_t cnt_rem_data_part;//количество оставшихся частей данных для передачи
	volatile uint32_t total_count_bytes;//количество переданных байтов документа
	volatile uint8_t http_doc;//вариант документа для передачи
	volatile uint8_t prt_tp;//вариант документа для передачи
	char fname[20];//имя файла (документа)
} http_sock_prop_ptr;

//--------------------------------------------------
//Варианты документов HTTP
#define EXISTING_HTML 0
#define E404_HTML 1
#define EXISTING_JPG 2
#define EXISTING_ICO 3

//--------------------------------------------------
void http_request(void);

#ifdef __cplusplus
}
#endif

#endif /* HTTPD_H_ */