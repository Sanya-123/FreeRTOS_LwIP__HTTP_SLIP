/**
  ******************************************************************************
  * @file           ustdio.c
  * @author         ÑîÏèÕ¿
  * @brief          ·Ç±ê×Œ»¯ŽòÓ¡Êä³ö
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 GoodMorning
  *
  ******************************************************************************
  */
/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "ustdio.h"



fnFmtOutDef current_puts = NULL;
fnFmtOutDef default_puts = NULL;


const char none        []= "\033[0m";  
const char black       []= "\033[0;30m";  
const char dark_gray   []= "\033[1;30m";  
const char blue        []= "\033[0;34m";  
const char light_blue  []= "\033[1;34m";  
const char green       []= "\033[0;32m";  
const char light_green []= "\033[1;32m";  
const char cyan        []= "\033[0;36m";  
const char light_cyan  []= "\033[1;36m";  
const char red         []= "\033[0;31m";  
const char light_red   []= "\033[1;31m";  
const char purple      []= "\033[0;35m";  
const char light_purple[]= "\033[1;35m";  
const char brown       []= "\033[0;33m";  
const char yellow      []= "\033[1;33m";  
const char light_gray  []= "\033[0;37m";  
const char white       []= "\033[1;37m"; 

char * default_color = (char *)none;



/**
	* @author   ¹ÅÃŽÄþ
	* @brief    ÖØ¶šÒå printf º¯Êý¡£±ŸÉíÀŽËµ printf ·œ·šÊÇ±ÈœÏÂýµÄ£¬
	*           ÒòÎª printf Òª×öžü¶àµÄžñÊœÅÐ¶Ï£¬Êä³öµÄžñÊœžü¶àÒ»Ð©¡£
	*           ËùÒÔÎªÁËÐ§ÂÊ£¬ÔÚºóÃæÐŽÁË printk º¯Êý¡£
	* @return   NULL
*/
#if 1
#pragma import(__use_no_semihosting)             
//±ê×Œ¿âÐèÒªµÄÖ§³Öº¯Êý                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//¶šÒå_sys_exit()ÒÔ±ÜÃâÊ¹ÓÃ°ëÖ÷»úÄ£Êœ    
void _sys_exit(int x) 
{ 
	x = x; 
}

//ÖØ¶šÒåfputcº¯Êý 
int fputc(int ch, FILE *f)
{
	char  cChar = (char)ch;
	if (current_puts)
		current_puts(&cChar,1);
	return ch;
}
#endif 




/**
	* @author   ¹ÅÃŽÄþ
	* @brief    i_itoa 
	*           ÕûÐÍ×ªÊ®œøÖÆ×Ö·ûŽ®
	* @param    strbuf   ×ª×Ö·ûŽ®ËùÔÚÄÚŽæ
	* @param    value  Öµ
	* @return   ×ª»»ËùµÃ×Ö·ûŽ®³€¶È
*/	
int i_itoa(char * strbuf,int value)		
{
	int len = 0;
	int value_fix = (value<0)?(0-value) : value; 
	
	do
	{
		strbuf[len++] = (char)(value_fix % 10 + '0'); 
		value_fix = value_fix/10;
	}
	while(value_fix);
	
	if (value < 0) 
		strbuf[len++] = '-'; 
	
	for (uint8_t index = 1 ; index <= len/2; ++index)
	{
		char reverse = strbuf[len  - index];  
		strbuf[len - index] = strbuf[index -1];   
		strbuf[index - 1] = reverse; 
	}
	
	return len;
}




/**
	* @author   ¹ÅÃŽÄþ
	* @brief    i_ftoa 
	*           ž¡µãÐÍ×ª×Ö·ûŽ®£¬±£Áô4Î»Ð¡Êý
	* @param    strbuf   ×ª×Ö·ûŽ®ËùÔÚÄÚŽæ
	* @param    value  Öµ
	* @return   ×Ö·ûŽ®³€¶È
*/
int i_ftoa(char * strbuf,float value)		
{
	int len = 0;
	float value_fix = (value < 0.0f )? (0.0f - value) : value; 
	int int_part   = (int)value_fix;  
	int float_part =  (int)(value_fix * 10000) - int_part * 10000;

	for(uint32_t i = 0 ; i < 4 ; ++i)
	{		
		strbuf[len++] = (char)(float_part % 10 + '0');
		float_part = float_part / 10;
	}
	
	strbuf[len++] = '.';  

	do
	{
		strbuf[len++] = (char)(int_part % 10 + '0'); 
		int_part = int_part/10;
	}
	while(int_part);            
	
	
	if (value < 0.0f) 
		strbuf[len++] = '-'; 
	
	for (uint8_t index = 1 ; index <= len/2; ++index)
	{
		char reverse = strbuf[len  - index];  
		strbuf[len - index] = strbuf[index -1];   
		strbuf[index - 1] = reverse; 
	}
	
	return len;
}


/**
	* @author   ¹ÅÃŽÄþ
	* @brief    i_itoa 
	*           ÕûÐÍ×ªÊ®ÁùœøÖÆ×Ö·ûŽ®
	* @param    strbuf   ×ª×Ö·ûŽ®ËùÔÚÄÚŽæ
	* @param    value  Öµ
	* @return   ×ª»»ËùµÃ×Ö·ûŽ®³€¶È
*/	
int i_xtoa(char * strbuf,uint32_t value)		
{
	int len = 0;
	
	do{
		char ascii = (char)((value & 0x0f) + '0');
		strbuf[len++] = (ascii > '9') ? (ascii + 7) : (ascii);
		value >>= 4;
	}
	while(value);
	
	for (uint8_t index = 1 ; index <= len/2; ++index)
	{
		char reverse = strbuf[len  - index];  
		strbuf[len - index] = strbuf[index -1];   
		strbuf[index - 1] = reverse; 
	}
	
	return len;
}



/**
	* @author   ¹ÅÃŽÄþ
	* @brief    printk 
	*           žñÊœ»¯Êä³ö£¬ÀàËÆÓÚ sprintf ºÍ printf ,¿ÉÖØÈë
	*           ÓÃ±ê×Œ¿âµÄ sprintf ºÍ printf µÄ·œ·šÌ«ÂýÁË£¬ËùÒÔ×ÔŒºÐŽÁËÒ»žö£¬ÖØµãÒª¿ì
	* @param    fmt     ÒªžñÊœ»¯µÄÐÅÏ¢×Ö·ûŽ®ÖžÕë
	* @param    ...     ²»¶š²Î
	* @return   void
*/
void printk(char* fmt, ...)
{
	char * buf_head = fmt;
	char * buf_tail = fmt;

	if (NULL == current_puts) 
		return ;
	
	va_list ap; 
	va_start(ap, fmt);

	while (*buf_tail) //ÐèÒª·ÀÖ¹·¢ËÍ»ºŽæÒç³ö
	{
		if ('%' == *buf_tail) //ÓöµœžñÊœ»¯±êÖŸ,ÎªÁËÐ§ÂÊœöÖ§³Ö %d ,%f ,%s %x ,%c 
		{
			char  buf_malloc[64] = { 0 };//°ÑÊý×Ö×ªÎª×Ö·ûŽ®µÄ»º³åÇø
			char *buf = buf_malloc;  //°ÑÊý×Ö×ªÎª×Ö·ûŽ®µÄ»º³åÇø
			int   len = 0;   //×îÖÕ×ª»»³€¶È
			
			if (buf_tail != buf_head)//°Ñ % Ç°ÃæµÄ²¿·ÖÊä³ö
				current_puts(buf_head,buf_tail - buf_head);
	
			buf_head = buf_tail++;
			switch (*buf_tail++) // Ÿ­¹ýÁœŽÎ ++, buf_tail ÒÑÔœ¹ý %? 
			{
				case 'd':
					len = i_itoa(buf,va_arg(ap, int));
					break;

				case 'f':
					len = i_ftoa(buf,(float)va_arg(ap, double));
					break;

				case 'x':
					len = i_xtoa(buf,va_arg(ap, int));
					break;
					
				case 'c' :
					buf[len++] = (char)va_arg(ap, int);
					break;
				
				case 's':
					buf = va_arg(ap, char*);
					len = strlen(buf);
					break;

				default:continue;
			}
			
			buf_head = buf_tail;
			current_puts(buf,len);
		}
		else
		{
			++buf_tail;
		}
	}

	va_end(ap);
	
	if (buf_tail != buf_head) 
		current_puts(buf_head,buf_tail - buf_head);
}



