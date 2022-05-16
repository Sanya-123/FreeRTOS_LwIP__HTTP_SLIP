/**
  ******************************************************************************
  * @file           telent_server.c
  * @author         №ЕГґДю
  * @brief          telent ·юОсЖч
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 GoodMorning
  *
  ******************************************************************************
  */
/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
//#include "cmsis_os.h"
#include "avltree.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/apps/fs.h" 

#include "shell.h"
#include "ustdio.h"
//#include "fatfs.h"


/* Private macro ------------------------------------------------------------*/

#define TELNET_WILL  251
#define TELNET_WONT  252
#define TELNET_DO    253
#define TELNET_DONT  254
#define TELNET_IAC   255


#define TELNET_NORMAL	   0
#define TELNET_BIN_TRAN    1
#define TELNET_BIN_ERROR   2



#define TELNET_FILE_ADDR 0x8060000
/*
* secureCRT telnet ·ўЛНОДјюґуёЕБчіМЈє
* CRT   : will bin tran ; do bin tran
* server: do bin tran
* CRT   : will bin tran 
* CRT   : <file data>
* CRT   : won't bin tran ; don't bin tran
* server: won't bin tran ; don't bin tran
* CRT   : won't bin tran 
* server: won't bin tran 
* CRT   : <string mode>
*/
	
/* Private types ------------------------------------------------------------*/




/* Private variables ------------------------------------------------------------*/

static struct telnetbuf
{
	struct netconn * conn;  //¶ФУ¦µД netconn ЦёХл
	volatile uint32_t tail; //telnet КэѕЭ»єіеЗшД©ОІ
	char buf[TCP_MSS];    //telnet КэѕЭ»єіеЗш __align(4) 
}
current_telnet, //µ±З°ХэФЪґ¦АнµД telnet Б¬ЅУ
bind_telnet;    //°у¶ЁБЛ printf/printk µД telnet Б¬ЅУ


static struct telnetfile
{
	uint16_t skip0xff;
	uint16_t remain ;
	uint32_t addr ;
	char buf[TCP_MSS+4];
}
telnet_file;

//static char telnet_state = TELNET_NORMAL; //



/*---------------------------------------------------------------------------*/

static void telnet_puts(char * buf,uint16_t len)
{
	struct telnetbuf * putsbuf;

	if ( current_telnet.conn )     //µ±З°ХэФЪґ¦Ан telnet Б¬ЅУ
		putsbuf = &current_telnet;
	else
	if ( bind_telnet.conn )      //°у¶Ё printf/printk µД telnet Б¬ЅУ
		putsbuf = &bind_telnet;
	else
		return ;
	
	if ( putsbuf->tail + len < TCP_MSS) 
	{
		memcpy(&putsbuf->buf[putsbuf->tail],buf,len);
		putsbuf->tail += len;
	}
}




/**
	* @brief    telnet_option 
	*           telnet ЕдЦГ
	* @param    arg ИООсІОКэ
	* @return   void
*/
static void telnet_option(uint8_t option, uint8_t value) //telnet_option(TELNET_DONT,1)
{
	volatile uint32_t new_tail = 3 + current_telnet.tail;
	
	if ( new_tail < TCP_MSS ) 
	{
		char * buf = &current_telnet.buf[current_telnet.tail];
		*buf = (char)TELNET_IAC;
		*++buf = (char)option;
		*++buf = (char)value;
		current_telnet.tail = new_tail;
	}
}



/**
	* @brief    telnet_check_option 
	*           telnet Б¬ЅУК±РиТЄјмІв»ШёґїН»§¶ЛµДСЎПоЕдЦГ
	* @param    arg ИООсІОКэ
	* @return   void
*/
void telnet_check_option(char ** telnetbuf , uint16_t * buflen ,uint32_t * telnetstate)
{
	uint8_t iac = (uint8_t)((*telnetbuf)[0]);
	uint8_t opt = (uint8_t)((*telnetbuf)[1]);
	uint8_t val = (uint8_t)((*telnetbuf)[2]);

	if (TELNET_NORMAL == *telnetstate)
	{
		while(iac == TELNET_IAC && opt > 250 )
		{
			if (0 == val) //Ц»»Шёґ¶юЅшЦЖГьБо
			{
				if (TELNET_WILL == opt)
				{
					*telnetstate = TELNET_BIN_TRAN;
					telnet_file.addr = TELNET_FILE_ADDR;
					telnet_file.remain = 0;
					telnet_file.skip0xff = 0;
//					HAL_FLASH_Unlock();
				}
				else
					telnet_option(opt, val);
			}
			
			*telnetbuf += 3;
			*buflen -= 3;
			iac = (uint8_t)((*telnetbuf)[0]);
			opt = (uint8_t)((*telnetbuf)[1]);
			val = (uint8_t)((*telnetbuf)[2]);
		}
	}
	else
	{
		while(iac == TELNET_IAC && val == 0  && opt > 250 )
		{
			if (TELNET_WONT == opt) //Ц»»Шёґ¶юЅшЦЖГьБо
			{
				iac = (uint8_t)((*telnetbuf)[3]);
				opt = (uint8_t)((*telnetbuf)[4]);
				val = (uint8_t)((*telnetbuf)[5]);

				if ( iac == TELNET_IAC  && opt == TELNET_DONT  && val == 0 )
				{
//					HAL_FLASH_Lock();
					telnet_option(TELNET_WONT, 0);//НЛіц¶юЅшЦЖґ«КдДЈКЅ
					telnet_option(TELNET_DONT, 0);//НЛіц¶юЅшЦЖґ«КдДЈКЅ
					char * msg = & current_telnet.buf[current_telnet.tail];
					sprintf(msg,"\r\nGet file,size=%d bytes\r\n",telnet_file.addr-TELNET_FILE_ADDR);
					current_telnet.tail += strlen(msg);
					*telnetbuf += 3;
					*buflen -= 3;
					*telnetstate = TELNET_NORMAL;
				}
				else
					return ;
			}
			
			*telnetbuf += 3;
			*buflen -= 3;
			iac = (uint8_t)((*telnetbuf)[0]);
			opt = (uint8_t)((*telnetbuf)[1]);
			val = (uint8_t)((*telnetbuf)[2]);
		}
	}
}



/**
	* @brief    telnet_recv_file 
	*           telnet ЅУКХОДјюЈ¬ґжУЪ flash ЦР
	* @param    arg ИООсІОКэ
	* @return   void
*/
void telnet_recv_file(char * data , uint16_t len)
{
	uint8_t  * copyfrom = (uint8_t*)data ;//+ telnet_file.skip0xff;//0xff 0xff ±»·Ц°ьµДЗйїцЈ¬Мш№эµЪТ»ёц ff
	uint8_t	 * copyend = copyfrom + len ;
	uint8_t  * copyto = (uint8_t*)(&telnet_file.buf[telnet_file.remain]);
	uint32_t * value = (uint32_t*)(&telnet_file.buf[0]);
	uint32_t   size = 0;
	
	//telnet_file.skip0xff = ((uint8_t)data[len-1] == 0xff && (uint8_t)data[len-2] != 0xff);//0xff 0xff ±»·Ц°ьµДЗйїц

	//Из№ыОДјюЦРґжФЪ 0xff Ј¬ФЪ SecureCRT »б·ўБЅёц 0xff Ј¬РиТЄМЮіэТ»ёц
	while(copyfrom < copyend)
	{
		*copyto++ = *copyfrom++ ;
		if (*copyfrom == 0xff) 
			++copyfrom;
	}

	size = copyto - (uint8_t*)(&telnet_file.buf[0]);
	telnet_file.remain = size & 0x03 ;//stm32f429 µД flash ТФ4µДХыКэ±¶РґИлЈ¬І»Чг4ЧЦЅЪБфµЅПВТ»°ьРґИл 
	size >>= 2; 	                  // іэУЪ 4
	
	for(uint32_t i = 0;i < size ; ++i)
	{
        if (/*HAL_OK != HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,telnet_file.addr,*value)*/0)
		{
			Errors("write data error\r\n");
		}
		else
		{
			++value;
			telnet_file.addr += 4;
		}
	}
	
	if (telnet_file.remain) //ґЛґОГ»РґНкµДБфµЅПВґОРґ
		memcpy(telnet_file.buf,&telnet_file.buf[size<<2],telnet_file.remain);
}



/**
	* @brief    telnet_recv_pro 
	*           telnet ЅУКХКэѕЭИООс
	* @param    arg ИООсІОКэ
	* @return   void
*/
static void telnet_recv_pro(void const * arg)
{
	uint32_t state = TELNET_NORMAL ;
	struct netconn	* this_conn = (struct netconn *)arg; //µ±З° telnet Б¬ЅУѕд±ъ	
	struct netbuf	* recvbuf;
	struct shell_buf  telnet_shell;//РВЅЁ shell Ѕ»»Ґ 
	
	char shell_bufmem[COMMANDLINE_MAX_LEN] = {0};

	telnet_shell.bufmem = shell_bufmem;
	telnet_shell.index = 0;
	telnet_shell.puts = telnet_puts;//¶ЁТе telnet µД shell КдИлєНКдіц
	
	telnet_option(TELNET_DO,1);   //їН»§¶ЛїЄЖф»ШПФ
	//telnet_option(TELNET_DO,34);//їН»§¶Л№Ш±ХРРДЈКЅ
		
	while(ERR_OK == netconn_recv(this_conn, &recvbuf))	//ЧиИыЦ±µЅКХµЅКэѕЭ
	{
		current_telnet.conn = this_conn;
		
		do
		{
			char *	  recvdata;
			uint16_t  datalen;
			
			netbuf_data(recvbuf, (void**)&recvdata, &datalen); //МбИЎКэѕЭЦёХл
			
			telnet_check_option(&recvdata,&datalen,&state);

			if (datalen)
			{
				if (TELNET_NORMAL == state)
					shell_input(&telnet_shell,recvdata,datalen);//°СКэѕЭУл shell Ѕ»»Ґ
				else
					telnet_recv_file(recvdata,datalen);
			}
		}
		while(netbuf_next(recvbuf) > 0);
		
		netbuf_delete(recvbuf);
		
		current_telnet.conn = NULL;
		
		if ( current_telnet.tail ) // Из№ы telnet »єіеЗшУРКэѕЭЈ¬·ўЛН
		{	
			netconn_write(this_conn,current_telnet.buf,current_telnet.tail, NETCONN_COPY);
			current_telnet.tail = 0;
		}
		
		if ((!bind_telnet.conn) && (default_puts == telnet_puts)) //КдИлБЛ debug-info »сИЎРЕПўБчєу
		{
			bind_telnet.conn = this_conn;//°у¶Ё telnet КэѕЭБчПт
			bind_telnet.tail = 0;
		}
	}

	if (this_conn == bind_telnet.conn)// №Ш±Х telnet К±Из№ыУРКэѕЭБч°у¶ЁЈ¬Ѕв°у
	{
		bind_telnet.conn = NULL;
		default_puts = NULL;
	}
	
	netconn_close(this_conn); //№Ш±ХБґЅУ
	netconn_delete(this_conn);//КН·ЕБ¬ЅУµДДЪґж
	
	vTaskDelete(NULL);//Б¬ЅУ¶ПїЄК±ЙѕіэЧФјє
}




/**
	* @brief    telnet_server_listen 
	*           telnet ·юОсЖчјаМэИООс
	* @param    void
	* @return   void
*/
static void telnet_server_listen(void const * arg)
{
    vTaskDelay(500);
	struct netconn *conn, *newconn;
	err_t err;

	conn = netconn_new(NETCONN_TCP); //ґґЅЁТ»ёц TCP БґЅУ
	netconn_bind(conn,IP_ADDR_ANY,23); //°у¶Ё¶ЛїЪ 23 єЕ¶ЛїЪ
    LWIP_ERROR("Telnet_server: invalid conn", (conn != NULL), (NULL););
	netconn_listen(conn); //ЅшИлјаМэДЈКЅ


	for(;;)
	{
		err = netconn_accept(conn,&newconn); //ЅУКХБ¬ЅУЗлЗу
		
		if (err == ERR_OK) //РВБ¬ЅУіЙ№¦К±Ј¬їЄ±ЩРВТ»ёцПЯіМґ¦Ан telnet
		{
//		  osThreadDef(telnet, telnet_recv_pro, osPriorityNormal, 0, 512);
//		  osThreadCreate(osThread(telnet), newconn);
          xTaskCreate(telnet_recv_pro, "AbonentTelNet", 512, (void*)newconn, 2, NULL);
		}
	}
}



/**
	* @brief    telnet_idle_pro 
	*           telnet їХПРґ¦АнєЇКэ
	* @param    void
	* @return   void
*/
void telnet_idle_pro(void)
{
    vTaskDelay(1000);
    while(1)
    {
        if (bind_telnet.conn && bind_telnet.tail)
        {
            netconn_write(bind_telnet.conn,bind_telnet.buf,bind_telnet.tail, NETCONN_COPY);
            bind_telnet.tail = 0;
        }
    }
}



/**
	* @brief    telnet_erase_file 
	*           telnet ЗеїХЅУКХµЅµДОДјю
	* @param    arg ИООсІОКэ
	* @return   void
*/
void telnet_erase_file(void * arg)
{
    (void)arg;
//	uint32_t SectorError;
//    FLASH_EraseInitTypeDef FlashEraseInit;
	
//	FlashEraseInit.TypeErase    = FLASH_TYPEERASE_SECTORS; //ІБіэАаРНЈ¬ЙИЗшІБіэ
//	FlashEraseInit.Sector       = 7;                       //0x8060000 ФЪ F429 ЙИЗш7Ј¬ІБіэ
//	FlashEraseInit.NbSectors    = 1;                       //Т»ґОЦ»ІБіэТ»ёцЙИЗш
//	FlashEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;   //µзС№·¶О§Ј¬VCC=2.7~3.6VЦ®јд!!
	
//	HAL_FLASH_Unlock();
//	HAL_FLASHEx_Erase(&FlashEraseInit,&SectorError);
//	HAL_FLASH_Lock();
    printk("done\r\n");
    xprintf("done\r\n");
}



/**
	* @brief    telnet_server_init 
	*           telnet ·юОсЖч¶ЛіхКј»Ї
	* @param    void
	* @return   void
*/
void telnet_server_init(void)
{
	current_telnet.tail = 0;
	bind_telnet.tail = 0;
	
	shell_register_command("telnet-erase",telnet_erase_file);	
	
//	osThreadDef(TelnetServer, telnet_server_listen, osPriorityNormal, 0, 128);
//	osThreadCreate(osThread(TelnetServer), NULL);//іхКј»ЇНк lwip єуґґЅЁtcp·юОсЖчјаМэИООс
    xTaskCreate(telnet_server_listen, "TelNet Server", 128, NULL, 2, NULL);
    xTaskCreate(telnet_idle_pro, "telNetIdle", 256, NULL, 1, NULL);
}


//void task_abonentTelNet(void *p);

//void theard_Telnet_Server(void *p)
//{
//    (void)p;
//    //create semafore abonent
//    semaforeAbonent = xSemaphoreCreateBinary();
//    xSemaphoreGive(semaforeAbonent);
//    vTaskDelay(500);

//    //***********************************init console***********************************
//    //init terminal
//    microrl_init (prl_T, printTelNet);
//    microrl_set_execute_callback (prl_T, execute);
//#ifdef _USE_COMPLETE
//    // set callback for completion
//    microrl_set_complete_callback (prl_T, complet);
//#endif
////	 set callback for Ctrl+C
//    microrl_set_sigint_callback (prl_T, sigint);
//    //set echo
//    microrl_set_echo(prl_T, true);//по умолдчанию тут есть эхо

//    //**********************************************************************

//    //**************************Telnet port**************************
//    struct netconn *conn, *newconn;
//    err_t err;

//    /* Create a new connection identifier. */
//    /* Bind connection to well known port number 7. */
//    conn = netconn_new(NETCONN_TCP);
//    netconn_bind(conn, IP_ADDR_ANY, TELNET_PORT);
//    LWIP_ERROR("Telnet_server: invalid conn", (conn != NULL), return;);

//    /* Tell connection to go into listening mode. */
//    netconn_listen(conn);
//    //****************************************************

//    xprintf("Init Telnet DONE!\n");

//    while(1)
//    {
//        vTaskDelay(5);
//        err = netconn_accept(conn, &newconn);
//        if (err != ERR_OK)
//        {
//            continue;
//        }
//        //else server
//        //create abonent
//        if(numAbonent < MAX_ABONENT_TELNET)
//        {
//            static uint32_t numConnect = 0;
//            char abonentN[12] = {0};
//            xsprintf(abonentN, "abonent %d", numConnect++);
//            if(xTaskCreate(task_abonentTelNet, abonentN, 1024, (void*)newconn, 2, NULL) == pdPASS)//если получилось создать таск
//            {
//                numAbonent++;
//            }
//            else//иначе закрываю соединение
//            {
//                netconn_close(newconn);
//                netconn_delete(newconn);
//            }
//        }
//        else//иначе закрываю соединение
//        {
//            xprintf("So more abonents now connect\n");
//            netconn_close(newconn);
//            netconn_delete(newconn);
//        }
//    }
//}

//void task_abonentTelNet(void *p)//таск абоненты для каждого будет выделять новый
//{
//    struct netconn *newconn = (struct netconn *)p;
//    struct netbuf *inbuf;
//    char *buf;
//    u16_t buflen;
//    err_t err;
//    xprintf("Begin seanse Telnet\n");

//    /* Read the data from the port, blocking if nothing yet there.
//     We assume the request (the part we care about) is in one netbuf */
//    do
//    {
//        err = netconn_recv(newconn, &inbuf);
//        if (err == ERR_OK)
//        {

//            while(xSemaphoreTake(semaforeAbonent, portMAX_DELAY) != pdTRUE);//жду пока освободиться семафора
//            newconnUse = newconn;//говорю что этот сеанс сейчас используеться
//            netbuf_data(inbuf, (void **)&buf, &buflen);
//            if(buflen > 2)//это он просит допуск
//            {
//                netconn_write(newconn, buf, buflen, NETCONN_NOCOPY);
//                microrl_insert_char(prl_T, '\r');//иметирую энтер чтобы появилась строка
//            }
//            else
//            {
//                //обращаюсь к обработчику
//                microrl_insert_char(prl_T, buf[0]);
//            }


//            //WARNING в той консоли и так присуствует эхо
//            for(uint32_t i = 0; i < _counter; i++)
//            {
//                netconn_write(newconn, (void*)(&buferTerminal[_rd_index]), (u16_t)1, NETCONN_NOCOPY);//отправяю данные сформирование microrl
//                _rd_index++;
//                if(_rd_index == TELNET_BUFFER_SIZE)
//                    _rd_index = 0;
//            }
//            _counter = 0;
//            newconnUse = NULL;//закончил использовать сеанс
//            xSemaphoreGive(semaforeAbonent);//все обработа  и отослал возвращаю семафору
//        }
//        else
//            break;
//    }
//    while(1);
//    //отключение от абонента
//    xprintf("Close seanse Telnet\n");
//    netconn_close(newconn);
//    netbuf_delete(inbuf);
//    netconn_delete(newconn);

//    numAbonent--;

//    //************************************info************************************
//    TaskStatus_t status;
//    vTaskGetInfo(NULL, &status, pdTRUE, eInvalid);

//    xprintf("+--------------------------------+\n");
//    xprintf("Task name:%s\n", status.pcTaskName);
//    xprintf("Size FREE:%d\n", status.usStackHighWaterMark);
//    xprintf("+--------------------------------+\n");
//    //************************************************************************

//    vTaskDelete(NULL);//по закрытию абонента завершаю таск
//    while(1);
//}




