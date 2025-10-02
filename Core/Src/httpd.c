#include "httpd.h"
#include "w5500.h"

//-----------------------------------------------
extern UART_HandleTypeDef huart2;

//-----------------------------------------------
extern char str1[60];
extern char tmpbuf[30];
extern uint8_t sect[515];

//-----------------------------------------------
#ifdef HTTP_support

http_sock_prop_ptr httpsockprop[2];
extern tcp_prop_ptr tcpprop;
FIL MyFile;
FRESULT result; //результат выполнения
uint32_t bytesread;
volatile uint16_t tcp_size_wnd = 2048;

//-----------------------------------------------
const char http_header[] = { "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"};
const char jpg_header[] = {"HTTP/1.0 200 OK\r\nServer: nginx\r\nContent-Type: image/jpeg\r\nConnection: close\r\n\r\n"};
const char icon_header[] = { "HTTP/1.1 200 OK\r\nContent-Type: image/x-icon\r\n\r\n"};
const char error_header[] = {"HTTP/1.0 404 File not found\r\nServer: nginx\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n"};
char *header;

//-----------------------------------------------
const uint8_t e404_htm[] = {
0x3c,0x68,0x74,0x6d,0x6c,0x3e,0x0a,0x20,0x20,0x3c,0x68,0x65,0x61,0x64,0x3e,0x0a,
0x20,0x20,0x20,0x20,0x3c,0x74,0x69,0x74,0x6c,0x65,0x3e,0x34,0x30,0x34,0x20,0x4e,
0x6f,0x74,0x20,0x46,0x6f,0x75,0x6e,0x64,0x3c,0x2f,0x74,0x69,0x74,0x6c,0x65,0x3e,
0x0a,0x20,0x20,0x3c,0x2f,0x68,0x65,0x61,0x64,0x3e,0x0a,0x3c,0x62,0x6f,0x64,0x79,
0x3e,0x0a,0x3c,0x68,0x31,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x74,0x65,0x78,
0x74,0x2d,0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,
0x22,0x3e,0x34,0x30,0x34,0x20,0x45,0x72,0x72,0x6f,0x72,0x20,0x46,0x69,0x6c,0x65,
0x20,0x4e,0x6f,0x74,0x20,0x46,0x6f,0x75,0x6e,0x64,0x3c,0x2f,0x68,0x31,0x3e,0x0a,
0x3c,0x68,0x32,0x20,0x73,0x74,0x79,0x6c,0x65,0x3d,0x22,0x74,0x65,0x78,0x74,0x2d,
0x61,0x6c,0x69,0x67,0x6e,0x3a,0x20,0x63,0x65,0x6e,0x74,0x65,0x72,0x3b,0x22,0x3e,
0x20,0x54,0x68,0x65,0x20,0x70,0x61,0x67,0x65,0x20,0x79,0x6f,0x75,0x20,0x61,0x72,
0x65,0x20,0x6c,0x6f,0x6f,0x6b,0x69,0x6e,0x67,0x20,0x66,0x6f,0x72,0x20,0x6d,0x69,
0x67,0x68,0x74,0x20,0x68,0x61,0x76,0x65,0x20,0x62,0x65,0x65,0x6e,0x20,0x72,0x65,
0x6d,0x6f,0x76,0x65,0x64,0x2c,0x20,0x3c,0x62,0x72,0x20,0x2f,0x3e,0x68,0x61,0x64,
0x20,0x69,0x74,0x73,0x20,0x6e,0x61,0x6d,0x65,0x20,0x63,0x68,0x61,0x6e,0x67,0x65,
0x64,0x2c,0x20,0x6f,0x72,0x20,0x69,0x73,0x20,0x74,0x65,0x6d,0x70,0x6f,0x72,0x61,
0x72,0x69,0x6c,0x79,0x20,0x75,0x6e,0x61,0x76,0x61,0x69,0x6c,0x61,0x62,0x6c,0x65,
0x2e,0x3c,0x2f,0x68,0x32,0x3e,0x0a,0x3c,0x2f,0x62,0x6f,0x64,0x79,0x3e,0x3c,0x2f,
0x68,0x74,0x6d,0x6c,0x3e};

//-----------------------------------------------
void tcp_send_http_one(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t header_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
	if ((httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_HTML)||\
		(httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_JPG)||\
		(httpsockprop[tcpprop.cur_sock].http_doc==EXISTING_ICO))
	{
		switch(httpsockprop[tcpprop.cur_sock].http_doc)
		{
			case EXISTING_HTML:
				header = (void*)http_header;
				break;
			case EXISTING_ICO:
				header = (void*)icon_header;
				break;
			case EXISTING_JPG:
				header = (void*)jpg_header;
				break;
		}
		header_len = strlen(header);
		data_len = (uint16_t)MyFile.fsize;
		end_point = GetWritePointer(tcpprop.cur_sock);
		end_point+=header_len+data_len;
		//Заполним данными буфер для отправки пакета
		SetWritePointer(tcpprop.cur_sock, end_point);
		end_point = GetWritePointer(tcpprop.cur_sock);
		memcpy(sect+3,header,header_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
		end_point+=header_len;
		num_sect = data_len / 512;
		for(i=0;i<=num_sect;i++)
		{
			//не последний сектор
			if(i<(num_sect-1)) len_sect=512;
			else len_sect=data_len;
			result=f_lseek(&MyFile,i*512); //Установим курсор чтения в файле
			sprintf(str1,"f_lseek: %d\r\n",result);
			HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
			result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
			w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
			end_point+=len_sect;
			data_len -= len_sect;
		}
	}
	else
	{
		header_len = strlen(error_header);
		data_len = sizeof(e404_htm);
		end_point = GetWritePointer(tcpprop.cur_sock);
		end_point+=header_len+data_len;
		SetWritePointer(tcpprop.cur_sock, end_point);
		end_point = GetWritePointer(tcpprop.cur_sock);
		//Заполним данными буфер для отправки пакета
		memcpy(sect+3,error_header,header_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
		end_point+=header_len;
		memcpy(sect+3,e404_htm,data_len);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, data_len);
		end_point+=data_len;
	}
	//отправим данные
  RecvSocket(tcpprop.cur_sock);
  SendSocket(tcpprop.cur_sock);
  httpsockprop[tcpprop.cur_sock].data_stat = DATA_COMPLETED;
}
//-----------------------------------------------
void tcp_send_http_first(void)
{
  uint8_t prt;
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t header_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
  uint16_t last_part, last_part_size;
	//На EXISTING проверять не будем, так как будем считать, что error404_htm у нас всегда будет умещаться в один буфер
	switch(httpsockprop[tcpprop.cur_sock].http_doc)
	{
		case EXISTING_HTML:
			header = (void*)http_header;
			break;
		case EXISTING_ICO:
			header = (void*)icon_header;
			break;
		case EXISTING_JPG:
			header = (void*)jpg_header;
			break;
	}
	header_len = strlen(header);
	data_len = tcp_size_wnd-header_len;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=header_len+data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//сохраним некоторые параметры, а то почему-то они теряются
	last_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
	last_part_size = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	prt = httpsockprop[tcpprop.cur_sock].prt_tp;
	//Заполним данными буфер для отправки пакета
	memcpy(sect+3,header,header_len);
	w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, header_len);
	end_point+=header_len;
	num_sect = data_len / 512;
	for(i=0;i<=num_sect;i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) len_sect=512;
		else len_sect=data_len;
		result=f_lseek(&MyFile,i*512); //Установим курсор чтения в файле
		sprintf(str1,"f_lseek: %d\r\n",result);
		HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
		result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//вернем параметры
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = last_part;
	httpsockprop[tcpprop.cur_sock].last_data_part_size = last_part_size;
	httpsockprop[tcpprop.cur_sock].prt_tp = prt;
	//отправим данные
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	//будем считать, что одну часть отправили, поэтому количество оставшихся частей декрементируем
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
	if(httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
	{   httpsockprop[tcpprop.cur_sock].data_stat=DATA_MIDDLE;   }
	else
	{   httpsockprop[tcpprop.cur_sock].data_stat=DATA_LAST; }
	//Количество переданных байтов
  httpsockprop[tcpprop.cur_sock].total_count_bytes = tcp_size_wnd - header_len;
}

//-----------------------------------------------
void tcp_send_http_middle(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint8_t prt;
  uint16_t len_sect;
  uint16_t last_part, last_part_size;
  uint32_t count_bytes;
	data_len = tcp_size_wnd;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//сохраним некоторые параметры, а то почему-то они теряются
	last_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
	last_part_size = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	count_bytes = httpsockprop[tcpprop.cur_sock].total_count_bytes;
	prt = httpsockprop[tcpprop.cur_sock].prt_tp;
	//Заполним данными буфер для отправки пакета
	num_sect = data_len / 512;
	//борьба с неправильным расчётом, когда размер данных делится на размер сектора без остатка
	if(data_len%512==0) num_sect--;
	for(i=0;i<=num_sect;i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) len_sect=512;
		else len_sect=data_len;
		result=f_lseek(&MyFile,(DWORD)(i*512) + count_bytes); //Установим курсор чтения в файле
		result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//вернем параметры
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = last_part;
	httpsockprop[tcpprop.cur_sock].last_data_part_size = last_part_size;
	httpsockprop[tcpprop.cur_sock].total_count_bytes = count_bytes;
	httpsockprop[tcpprop.cur_sock].prt_tp = prt;
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	//будем считать, что одну часть отправили, поэтому количество оставшихся частей декрементируем
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
	if(httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
	{   httpsockprop[tcpprop.cur_sock].data_stat=DATA_MIDDLE;   }
	else
	{   httpsockprop[tcpprop.cur_sock].data_stat=DATA_LAST; }
	//Количество переданных байтов
	httpsockprop[tcpprop.cur_sock].total_count_bytes += (uint32_t) tcp_size_wnd;
}

//-----------------------------------------------
void tcp_send_http_last(void)
{
  uint16_t i=0;
  uint16_t data_len=0;
  uint16_t end_point;
  uint8_t num_sect=0;
  uint16_t len_sect;
	data_len = httpsockprop[tcpprop.cur_sock].last_data_part_size;
	end_point = GetWritePointer(tcpprop.cur_sock);
	end_point+=data_len;
	SetWritePointer(tcpprop.cur_sock, end_point);
	end_point = GetWritePointer(tcpprop.cur_sock);
	//Заполним данными буфер для отправки пакета
	num_sect = data_len / 512;
	//борьба с неправильным расчётом, когда размер данных делится на размер сектора без остатка
	if(data_len%512==0) num_sect--;
	for(i=0;i<=num_sect;i++)
	{
		//не последний сектор
		if(i<(num_sect-1)) {len_sect=512;}
		else {len_sect=data_len;}
		result=f_lseek(&MyFile, (DWORD)(i*512) + httpsockprop[tcpprop.cur_sock].total_count_bytes); //Установим курсор чтения в файле
		sprintf(str1,"f_lseek: %d\r\n",result);
		HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
		result=f_read(&MyFile,sect+3,len_sect,(UINT *)&bytesread);
		w5500_writeSockBuf(tcpprop.cur_sock, end_point, (uint8_t*)sect, len_sect);
		end_point+=len_sect;
		data_len -= len_sect;
	}
	//отправим данные
	RecvSocket(tcpprop.cur_sock);
	SendSocket(tcpprop.cur_sock);
	httpsockprop[tcpprop.cur_sock].data_stat = DATA_COMPLETED;
}

//-----------------------------------------------
void http_request(void)
{
  uint16_t point;
  uint8_t RXbyte;
  uint16_t i=0;
  char *ss1;
  int ch1='.';
 
  // ищем первый "/" в HTTP заголовке
  point = GetReadPointer(tcpprop.cur_sock);
  i = 0;
  while (RXbyte != (uint8_t)'/')    {
    RXbyte = w5500_readSockBufByte(tcpprop.cur_sock,point+i);
    i++;
  }
	point+=i;
	RXbyte = w5500_readSockBufByte(tcpprop.cur_sock,point);
	if(RXbyte==(uint8_t)' ')    {
		strcpy(httpsockprop[tcpprop.cur_sock].fname,"index.htm");
		httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_HTML;
	}
	else    {
		// ищем следующий пробел (" ") в HTTP заголовке, таким образом считывая имя документа из запроса
		i=0;
		while (1)
		{
			tmpbuf[i] = w5500_readSockBufByte(tcpprop.cur_sock, point+i);
			if(tmpbuf[i] == (uint8_t)' ') break;
			i++;
		}
		tmpbuf[i] = 0; //закончим строку
		strcpy(httpsockprop[tcpprop.cur_sock].fname,tmpbuf);
	}
	HAL_UART_Transmit(&huart2,(uint8_t*)httpsockprop[tcpprop.cur_sock].fname,strlen(httpsockprop[tcpprop.cur_sock].fname),0x1000);
	HAL_UART_Transmit(&huart2,(uint8_t*)"\r\n",2,0x1000);
	//f_close(&MyFile);
	//result=f_open(&MyFile,httpsockprop[tcpprop.cur_sock].fname,FA_READ); //Попытка открыть файл
	sprintf(str1,"f_open: %d\r\n",result);
	HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
	sprintf(str1,"f_size: %lu\r\n",MyFile.fsize);
	HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
	if (result==FR_OK)
	{
		//изучим расширение файла
		ss1 = strchr(httpsockprop[tcpprop.cur_sock].fname,ch1);
		ss1++;
		if (strncmp(ss1,"jpg", 3) == 0)
		{
			httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_JPG;
			//сначала включаем в размер размер заголовка
			httpsockprop[tcpprop.cur_sock].data_size = strlen(jpg_header);
		}
		if (strncmp(ss1,"ico", 3) == 0)
		{
			httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_ICO;
			//сначала включаем в размер размер заголовка
			httpsockprop[tcpprop.cur_sock].data_size = strlen(icon_header);
		}
		else
		{
			httpsockprop[tcpprop.cur_sock].http_doc = EXISTING_HTML;
			//сначала включаем в размер размер заголовка
			httpsockprop[tcpprop.cur_sock].data_size = strlen(http_header);
		}
		//затем размер самого документа
		httpsockprop[tcpprop.cur_sock].data_size += MyFile.fsize;
	}
	else
	{
		httpsockprop[tcpprop.cur_sock].http_doc = E404_HTML;
		//сначала включаем в размер размер заголовка
		httpsockprop[tcpprop.cur_sock].data_size = strlen(error_header);
		//затем размер самого документа
		httpsockprop[tcpprop.cur_sock].data_size += sizeof(e404_htm);
	}
	httpsockprop[tcpprop.cur_sock].cnt_rem_data_part = httpsockprop[tcpprop.cur_sock].data_size / tcp_size_wnd + 1;
	httpsockprop[tcpprop.cur_sock].last_data_part_size = httpsockprop[tcpprop.cur_sock].data_size % tcp_size_wnd;
	//борьба с неправильным расчётом, когда общий размер делится на минимальный размер окна без остатка
	if(httpsockprop[tcpprop.cur_sock].last_data_part_size==0)
	{
		httpsockprop[tcpprop.cur_sock].last_data_part_size=tcp_size_wnd;
		httpsockprop[tcpprop.cur_sock].cnt_rem_data_part--;
	}
	httpsockprop[tcpprop.cur_sock].cnt_data_part = httpsockprop[tcpprop.cur_sock].cnt_rem_data_part;
	sprintf(str1,"data size:%lu; cnt data part:%u; last_data_part_size:%u\r\n",
	(unsigned long)httpsockprop[tcpprop.cur_sock].data_size, httpsockprop[tcpprop.cur_sock].cnt_rem_data_part, httpsockprop[tcpprop.cur_sock].last_data_part_size);
	HAL_UART_Transmit(&huart2,(uint8_t*)str1,strlen(str1),0x1000);
	if (httpsockprop[tcpprop.cur_sock].cnt_rem_data_part==1)
	{
		httpsockprop[tcpprop.cur_sock].data_stat = DATA_ONE;
	}
	else if (httpsockprop[tcpprop.cur_sock].cnt_rem_data_part>1)
	{
		httpsockprop[tcpprop.cur_sock].data_stat = DATA_FIRST;
	}
	if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_ONE)
	{
		 tcp_send_http_one();
		DisconnectSocket(tcpprop.cur_sock); //Разъединяемся
		SocketClosedWait(tcpprop.cur_sock);
		OpenSocket(tcpprop.cur_sock,Mode_TCP);
		//Ждём инициализации сокета (статус SOCK_INIT)
		SocketInitWait(tcpprop.cur_sock);
		//Продолжаем слушать сокет
		ListenSocket(tcpprop.cur_sock);
		SocketListenWait(tcpprop.cur_sock);
	}
	else if(httpsockprop[tcpprop.cur_sock].data_stat==DATA_FIRST)
	{
		tcp_send_http_first();
	}
}
//-----------------------------------------------

#endif


