/**
  ******************************************************************************
  * @file           shell.c
  * @author         №ЕГґДю
  * @brief          shell ГьБоЅвКНЖч
  ******************************************************************************
  *
  * COPYRIGHT(c) 2018 GoodMorning
  *
  ******************************************************************************
  */
/* Includes ---------------------------------------------------*/
#include <string.h>
#include <stdarg.h>
#include <stdint.h> //¶ЁТеБЛєЬ¶аКэѕЭАаРН
#include <stdio.h>
#include "shell.h"

//--------------------Па№Шєк¶ЁТеј°Ѕб№№Ме¶ЁТе--------------------
const static  uint8_t F_CRC8_Table[256] = {//ХэРт,ёЯО»ПИРР x^8+x^5+x^4+1
	0x00, 0x31, 0x62, 0x53, 0xc4, 0xf5, 0xa6, 0x97, 0xb9, 0x88, 0xdb, 0xea, 0x7d, 0x4c, 0x1f, 0x2e,
	0x43, 0x72, 0x21, 0x10, 0x87, 0xb6, 0xe5, 0xd4, 0xfa, 0xcb, 0x98, 0xa9, 0x3e, 0x0f, 0x5c, 0x6d,
	0x86, 0xb7, 0xe4, 0xd5, 0x42, 0x73, 0x20, 0x11, 0x3f, 0x0e, 0x5d, 0x6c, 0xfb, 0xca, 0x99, 0xa8,
	0xc5, 0xf4, 0xa7, 0x96, 0x01, 0x30, 0x63, 0x52, 0x7c, 0x4d, 0x1e, 0x2f, 0xb8, 0x89, 0xda, 0xeb,
	0x3d, 0x0c, 0x5f, 0x6e, 0xf9, 0xc8, 0x9b, 0xaa, 0x84, 0xb5, 0xe6, 0xd7, 0x40, 0x71, 0x22, 0x13,
	0x7e, 0x4f, 0x1c, 0x2d, 0xba, 0x8b, 0xd8, 0xe9, 0xc7, 0xf6, 0xa5, 0x94, 0x03, 0x32, 0x61, 0x50,
	0xbb, 0x8a, 0xd9, 0xe8, 0x7f, 0x4e, 0x1d, 0x2c, 0x02, 0x33, 0x60, 0x51, 0xc6, 0xf7, 0xa4, 0x95,
	0xf8, 0xc9, 0x9a, 0xab, 0x3c, 0x0d, 0x5e, 0x6f, 0x41, 0x70, 0x23, 0x12, 0x85, 0xb4, 0xe7, 0xd6,
	0x7a, 0x4b, 0x18, 0x29, 0xbe, 0x8f, 0xdc, 0xed, 0xc3, 0xf2, 0xa1, 0x90, 0x07, 0x36, 0x65, 0x54,
	0x39, 0x08, 0x5b, 0x6a, 0xfd, 0xcc, 0x9f, 0xae, 0x80, 0xb1, 0xe2, 0xd3, 0x44, 0x75, 0x26, 0x17,
	0xfc, 0xcd, 0x9e, 0xaf, 0x38, 0x09, 0x5a, 0x6b, 0x45, 0x74, 0x27, 0x16, 0x81, 0xb0, 0xe3, 0xd2,
	0xbf, 0x8e, 0xdd, 0xec, 0x7b, 0x4a, 0x19, 0x28, 0x06, 0x37, 0x64, 0x55, 0xc2, 0xf3, 0xa0, 0x91,
	0x47, 0x76, 0x25, 0x14, 0x83, 0xb2, 0xe1, 0xd0, 0xfe, 0xcf, 0x9c, 0xad, 0x3a, 0x0b, 0x58, 0x69,
	0x04, 0x35, 0x66, 0x57, 0xc0, 0xf1, 0xa2, 0x93, 0xbd, 0x8c, 0xdf, 0xee, 0x79, 0x48, 0x1b, 0x2a,
	0xc1, 0xf0, 0xa3, 0x92, 0x05, 0x34, 0x67, 0x56, 0x78, 0x49, 0x1a, 0x2b, 0xbc, 0x8d, 0xde, 0xef,
	0x82, 0xb3, 0xe0, 0xd1, 0x46, 0x77, 0x24, 0x15, 0x3b, 0x0a, 0x59, 0x68, 0xff, 0xce, 0x9d, 0xac
};

const static  uint8_t B_CRC8_Table[256] = {//·ґРт,µНО»ПИРР x^8+x^5+x^4+1
	0x00, 0x5e, 0xbc, 0xe2, 0x61, 0x3f, 0xdd, 0x83, 0xc2, 0x9c, 0x7e, 0x20, 0xa3, 0xfd, 0x1f, 0x41,
	0x9d, 0xc3, 0x21, 0x7f, 0xfc, 0xa2, 0x40, 0x1e, 0x5f, 0x01, 0xe3, 0xbd, 0x3e, 0x60, 0x82, 0xdc,
	0x23, 0x7d, 0x9f, 0xc1, 0x42, 0x1c, 0xfe, 0xa0, 0xe1, 0xbf, 0x5d, 0x03, 0x80, 0xde, 0x3c, 0x62,
	0xbe, 0xe0, 0x02, 0x5c, 0xdf, 0x81, 0x63, 0x3d, 0x7c, 0x22, 0xc0, 0x9e, 0x1d, 0x43, 0xa1, 0xff,
	0x46, 0x18, 0xfa, 0xa4, 0x27, 0x79, 0x9b, 0xc5, 0x84, 0xda, 0x38, 0x66, 0xe5, 0xbb, 0x59, 0x07,
	0xdb, 0x85, 0x67, 0x39, 0xba, 0xe4, 0x06, 0x58, 0x19, 0x47, 0xa5, 0xfb, 0x78, 0x26, 0xc4, 0x9a,
	0x65, 0x3b, 0xd9, 0x87, 0x04, 0x5a, 0xb8, 0xe6, 0xa7, 0xf9, 0x1b, 0x45, 0xc6, 0x98, 0x7a, 0x24,
	0xf8, 0xa6, 0x44, 0x1a, 0x99, 0xc7, 0x25, 0x7b, 0x3a, 0x64, 0x86, 0xd8, 0x5b, 0x05, 0xe7, 0xb9,
	0x8c, 0xd2, 0x30, 0x6e, 0xed, 0xb3, 0x51, 0x0f, 0x4e, 0x10, 0xf2, 0xac, 0x2f, 0x71, 0x93, 0xcd,
	0x11, 0x4f, 0xad, 0xf3, 0x70, 0x2e, 0xcc, 0x92, 0xd3, 0x8d, 0x6f, 0x31, 0xb2, 0xec, 0x0e, 0x50,
	0xaf, 0xf1, 0x13, 0x4d, 0xce, 0x90, 0x72, 0x2c, 0x6d, 0x33, 0xd1, 0x8f, 0x0c, 0x52, 0xb0, 0xee,
	0x32, 0x6c, 0x8e, 0xd0, 0x53, 0x0d, 0xef, 0xb1, 0xf0, 0xae, 0x4c, 0x12, 0x91, 0xcf, 0x2d, 0x73,
	0xca, 0x94, 0x76, 0x28, 0xab, 0xf5, 0x17, 0x49, 0x08, 0x56, 0xb4, 0xea, 0x69, 0x37, 0xd5, 0x8b,
	0x57, 0x09, 0xeb, 0xb5, 0x36, 0x68, 0x8a, 0xd4, 0x95, 0xcb, 0x29, 0x77, 0xf4, 0xaa, 0x48, 0x16,
	0xe9, 0xb7, 0x55, 0x0b, 0x88, 0xd6, 0x34, 0x6a, 0x2b, 0x75, 0x97, 0xc9, 0x4a, 0x14, 0xf6, 0xa8,
	0x74, 0x2a, 0xc8, 0x96, 0x15, 0x4b, 0xa9, 0xf7, 0xb6, 0xe8, 0x0a, 0x54, 0xd7, 0x89, 0x6b, 0x35
};





union uncmd
{
	struct // ГьБоєЕ·ЦОЄТФПВОеёцІї·Ц
	{
		uint32_t CRC2      : 8;
		uint32_t CRC1      : 8;//µНК®БщО»ОЄБЅёц crc РЈСйВл
		uint32_t Sum       : 5;//ГьБоЧЦ·ыµДЧЬєН
		uint32_t Len       : 5;//ГьБоЧЦ·ыµДі¤¶ИЈ¬5 bit Ј¬јґГьБоі¤¶ИІ»ДЬі¬№э31ёцЧЦ·ы
		uint32_t FirstChar : 6;//ГьБоЧЦ·ыµДµЪТ»ёцЧЦ·ы
	}part;

	uint32_t ID;//УЙґЛєПІўОЄ 32 О»µДГьБоВл
};


static struct avl_root shell_root = {.avl_node = NULL};//ГьБоЖҐЕдµДЖЅєв¶юІжКчКчёщ 

static struct _shell_record
{
	char  buf[COMMANDLINE_MAX_RECORD][COMMANDLINE_MAX_LEN];
	uint8_t read;
	uint8_t write;
}
shell_history = {0};

char shell_input_sign[128] = DEFAULT_INPUTSIGN;
//------------------------------Па№ШєЇКэЙщГч------------------------------

static char * shell_record(struct shell_buf * shellbuf);
void shell_getchar           (struct shell_buf * shellbuf , char ch);
void shell_backspace(struct shell_buf * shellbuf) ;
void shell_tab               (struct shell_buf * shellbuf) ;
void shell_show_history  (struct shell_buf * shellbuf ,uint8_t LastOrNext);


//------------------------------»ЄАцµД·ЦёоПЯ------------------------------
/**
	* @brief    pConsole_Search 
	*           ГьБоКчІйХТЈ¬ёщѕЭ id єЕХТµЅ¶ФУ¦µДїШЦЖїй
	* @param    CmdID        ГьБоєЕ
	* @return   іЙ№¦ id єЕ¶ФУ¦µДїШЦЖїй
*/
static struct shell_cmd *shell_search_cmd(int cmdindex)
{
    struct avl_node *node = shell_root.avl_node;

    while (node) 
	{
		struct shell_cmd * command = container_of(node, struct shell_cmd, cmd_node);

		if (cmdindex < command->ID)
		    node = node->avl_left;
		else 
		if (cmdindex > command->ID)
		    node = node->avl_right;
  		else 
			return command;
    }
    
    return NULL;
}



/**
	* @brief    shell_insert_cmd 
	*           ГьБоКчІеИл
	* @param    pCmd        ГьБоїШЦЖїй
	* @return   іЙ№¦·µ»Ш 0
*/
static int shell_insert_cmd(struct shell_cmd * newcmd)
{
	struct avl_node **tmp = &shell_root.avl_node;
 	struct avl_node *parent = NULL;
	
	/* Figure out where to put new node */
	while (*tmp)
	{
		struct shell_cmd *this = container_of(*tmp, struct shell_cmd, cmd_node);

		parent = *tmp;
		if (newcmd->ID < this->ID)
			tmp = &((*tmp)->avl_left);
		else 
		if (newcmd->ID > this->ID)
			tmp = &((*tmp)->avl_right);
		else
			return 1;
	}

	/* Add new node and rebalance tree. */
	//rb_link_node(&pCmd->cmd_node, parent, tmp);
	//rb_insert_color(&pCmd->cmd_node, root);
	avl_insert(&shell_root,&newcmd->cmd_node,parent,tmp);
	
	return 0;
}


/**
	* @author   №ЕГґДю
	* @brief    shell_getchar 
	*           ГьБоРРјЗВјКдИлТ»ёцЧЦ·ы
	* @param    
	* @return   
*/
void shell_getchar(struct shell_buf * shellbuf , char ascii)
{
	char * ptr = shellbuf->bufmem + shellbuf->index;
	shellbuf->bufmem[shellbuf->index] = ascii;
	shellbuf->index = (shellbuf->index + 1) % COMMANDLINE_MAX_LEN;
	shellbuf->bufmem[shellbuf->index] = 0;
	printl(ptr,1); //·ґАЎКдіцґтУЎ
}




/**
	* @author   №ЕГґДю
	* @brief    shell_backspace 
	*           їШЦЖМЁКдИл »ШНЛ јьґ¦Ан
	* @param    void
	* @return   void
*/
void shell_backspace(struct shell_buf * shellbuf)
{
	if (shellbuf->index)//Из№ыµ±З°ґтУЎРРУРКдИлДЪИЭЈ¬»ШНЛТ»ёцјьО»
	{
		printk("\010 \010"); //KEYCODE_BACKSPACE 
		shellbuf->bufmem[--shellbuf->index] = 0;
	}
}



/** 
	* @brief shell_tab КдИл table јьґ¦Ан
	* @param shellbuf
	*
	* @return NULL
*/
void shell_tab(struct shell_buf * shellbuf)
{
	uint32_t cnt ;
	
	uint32_t input_len = shellbuf->index;
	char  *  input_str = shellbuf->bufmem;
	uint32_t str_1st_char;
	
	struct shell_cmd * match[10];//ЖҐЕдµЅµДГьБоРР
	uint32_t           match_cnt = 0;//ЖҐЕдµЅµДГьБоєЕёцКэ
	
	while (*input_str == ' ')  //УРК±єт»бКдИлїХёсЈ¬РиТЄМш№э
	{
		++input_str;
		--input_len;
	}
	
	if (*input_str == 0 || input_len == 0) 
		return ;//Г»УРКдИлРЕПў·µ»Ш
	
	str_1st_char = (uint32_t)(*input_str)<<26;//ЖҐЕдКЧЧЦДё

    for (struct avl_node* node = avl_first(&shell_root); node ; node = avl_next(node))//±йАъ¶юІжКч
	{
		struct shell_cmd * shell_cmd = avl_entry(node,struct shell_cmd, cmd_node);
		uint32_t  cmd_1st_char = (shell_cmd->ID & (0xfc000000)); 
		
		if (str_1st_char == cmd_1st_char)//КЧЧЦДёПаН¬Ј¬ЖҐЕдГьБо
		{
			if (memcmp(shell_cmd->name, input_str, input_len) == 0) //¶Ф±ИГьБоЧЦ·ыґ®Ј¬Из№ыЖҐЕдµЅПаН¬µД
			{
				match[match_cnt] = shell_cmd;     //°СЖҐЕдµЅµДГьБоєЕЛчТэјЗПВАґ
				if (++match_cnt > 10) 
					return ;    //і¬№эК®МхПаН¬·µ»Ш
			}
		}
		else
		if (cmd_1st_char > str_1st_char) // УЙРЎµЅґу¶юІжКч±йАъЈ¬ЖҐЕдІ»µЅКЧЧЦДёНЛіцС­»·
		{
			break ;
		}
	}

	if (!match_cnt) 
		return ; //Из№ыГ»УРГьБо°ьє¬КдИлµДЧЦ·ыґ®Ј¬·µ»Ш
	
	if (1 == match_cnt)  //Из№ыЦ»ХТµЅБЛТ»МхГьБо°ьє¬µ±З°КдИлµДЧЦ·ыґ®Ј¬Ц±ЅУІ№И«ГьБоЈ¬ІўґтУЎ
	{
		for(char * ptr = match[0]->name + input_len ;*ptr ;++ptr) //ґтУЎКЈУаµДЧЦ·ы		
			shell_getchar(shellbuf,*ptr);
	}
	else   //Из№ыІ»Ц№Т»МхГьБо°ьє¬µ±З°КдИлµДЧЦ·ыґ®Ј¬ґтУЎє¬УРПаН¬ЧЦ·ыµДГьБоБР±нЈ¬ІўІ№И«ЧЦ·ыґ®КдіцЦ±µЅГьБоЗш·Цµг
	{
		for(cnt = 0;cnt < match_cnt; ++cnt) 
			printk("\r\n\t%s",match[cnt]->name); //°СЛщУРє¬УРКдИлЧЦ·ыґ®µДГьБоБР±нґтУЎіцАґ
		
		printk("\r\n%s%s",shell_input_sign,shellbuf->bufmem); //ЦШРВґтУЎКдИл±кЦѕєНТСКдИлµДЧЦ·ыґ®
		
		while(1)  //І№И«ГьБоЈ¬°СГїМхГьБо¶ј°ьє¬µДЧЦ·ыІ№И«ІўґтУЎ
		{
			for (cnt = 1;cnt < match_cnt; ++cnt)
			{
				if (match[0]->name[input_len] != match[cnt]->name[input_len]) 
					return  ; //ЧЦ·ыІ»Т»СщЈ¬·µ»Ш
			}
			shell_getchar(shellbuf,match[0]->name[input_len++]);  //°СПаН¬µДЧЦ·ыІ№И«µЅКдИл»єіеЦР
		}
	}
}






/**
	* @author   №ЕГґДю
	* @brief    shell_input 
	*           ГьБоРРЅвОцКдИл
	* @param    
	* @return   
*/
void shell_parse(struct shell_buf * shellbuf)
{
	uint32_t len = 0;
	uint32_t sum = 0;
	uint32_t fcrc8 = 0;
	uint32_t bcrc8 = 0;
	union uncmd unCmd ;
	
	char * input_str = shellbuf->bufmem;
	int    input_len = shellbuf->index ;
	
	struct shell_cmd * cmdmatch;
	
	while (*input_str == ' ')	// Shave off any leading spaces
	{
		++input_str;
		--input_len;
	}

	if (0 == input_str[0] || 0 == input_len)
		goto parseend;
	
	unCmd.part.FirstChar = *input_str;
	
	while ((*input_str != '\0') && (*input_str != ' '))
	{
		sum += *input_str;
		fcrc8 = F_CRC8_Table[fcrc8^*input_str];
		bcrc8 = B_CRC8_Table[bcrc8^*input_str];
		++input_str;
		++len;
	}
	
	unCmd.part.Len = len;
	unCmd.part.Sum = sum;
	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;
	
	cmdmatch = shell_search_cmd(unCmd.ID);//ЖҐЕдГьБоєЕ

	if (cmdmatch != NULL)
	{
		char * record = shell_record(shellbuf);  //јЗВјµ±З°КдИлµДГьБоєНГьБоІОКэ
		char * arg = record + SHELL_CMD_LEN(cmdmatch);
		cmdmatch->Func(arg);
	}
	else
	{
		printk("\r\n\r\n\tno reply:%s\r\n",shellbuf->bufmem);
	}
	
parseend:	
	shellbuf->index = 0;
	return ;
}




/**
	* @author   №ЕГґДю
	* @brief    shell_input 
	*           УІјюЙПЅУКХµЅµДКэѕЭµЅїШЦЖМЁјдµДґ«Кд
	* @param    pcHalRxBuf     УІјюІгЛщЅУКХµЅµДКэѕЭ»єіеЗшµШЦ·
	* @param    len          УІјюІгЛщЅУКХµЅµДКэѕЭі¤¶И
	* @return   void
*/
void shell_input(struct shell_buf * shellbuf,char * ptr,uint8_t len)
{
	current_puts = shellbuf->puts; //shell ИлїЪ¶ФУ¦іцїЪ
	
	for ( ; len && *ptr; --len,++ptr)
	{
		switch (*ptr) //ЕР¶ПЧЦ·ыКЗ·сОЄМШКвЧЦ·ы
		{
			case KEYCODE_NEWLINE: //єцВФ \r
				break;
				
			case KEYCODE_ENTER:
				printk("\r\n");
				if (shellbuf->index) 
					shell_parse(shellbuf);
				else
					printk("%s",shell_input_sign);
				break;
			
			case KEYCODE_TAB: 
				shell_tab(shellbuf); 
				break;
			
			case KEYCODE_BACKSPACE : 
				shell_backspace(shellbuf); 
				break;
			
			case KEYCODE_ESC :
				if (ptr[1] == 0x5b)
				{
					switch(ptr[2])
					{
						case 0x41:shell_show_history(shellbuf,0);break;//ЙПјэН·
						case 0x42:shell_show_history(shellbuf,1);break;//ПВјэН·
						default:;
					}
					
					len -= 2;
					ptr += 2;//јэН·УР3ёцЧЦЅЪЧЦ·ы
				}
				
				break;
				
			case KEYCODE_CTRL_C:
				printk("^C");
				shellbuf->index = 0;
				break;
			
			default: // ЖХНЁЧЦ·ы
				shell_getchar(shellbuf,*ptr); //КдИлµЅДЪґж»єіеЦР;
		}
	}
	
	current_puts = default_puts; //»ЦёґД¬ИПґтУЎ
}



/**
	* @author   №ЕГґДю
	* @brief    record 
	*           јЗВјґЛґОФЛРРµДГьБој°ІОКэ
	* @param    
	* @return   ·µ»ШјЗВјµШЦ·
*/
static char * shell_record(struct shell_buf * shellbuf)
{
	char *   input_str = shellbuf->bufmem;
	uint32_t input_len = shellbuf->index ;
	
	char *  history = &shell_history.buf[shell_history.write][0];
	
	shell_history.write = (shell_history.write + 1) % COMMANDLINE_MAX_RECORD;
	shell_history.read = shell_history.write;
	
	memcpy(history,input_str,input_len);
	history[input_len] = 0;
	
	return history;
}



/*******************************************************************
	* @author   №ЕГґДю
	* @brief    shell_show_history 
	*           °ґЙППВјэН·јьПФКѕТФНщКдИл№эµДГьБоЈ¬ґЛґ¦Ц»јЗВјЧоЅьјёґОµДГьБо
	* @param    void
	* @return   void
*/
void shell_show_history(struct shell_buf * shellbuf,uint8_t LastOrNext)
{
	uint32_t len;
	char *history;
	
	printk("\33[2K\r%s",shell_input_sign);//printk("\33[2K\r");±нКѕЗеіэµ±З°РР

	if (!LastOrNext) //ЙПјэН·Ј¬ЙПТ»МхГьБо
	{
		shell_history.read = (!shell_history.read) ? (COMMANDLINE_MAX_RECORD - 1) : (shell_history.read - 1);
	}
	else
	{	
		if (shell_history.read == shell_history.write)
		{
			shellbuf->bufmem[0] = 0;
			shellbuf->index = 0 ;
			return ;
		}
		
		shell_history.read = (shell_history.read + 1) % COMMANDLINE_MAX_RECORD;
	}
	
	history = &shell_history.buf[shell_history.read][0];
	len = strlen(history);
	if (len)
	{
		memcpy(shellbuf->bufmem,history,len);
		shellbuf->bufmem[len] = 0;
		shellbuf->index = len ;
		printl(shellbuf->bufmem,len);
	}
	else
	{
		shellbuf->bufmem[0] = 0;
		shellbuf->index = 0 ;
	}
}


/********************************************************************
	* @brief    shell_cmdparam 
	*           ЧЄ»»»сИЎГьБоєЕєуГжµДКдИлІОКэЈ¬ЧЦ·ыґ®ЧЄОЄХыКэ
	* @param    str       ГьБоЧЦ·ыґ®єуГжЛщёъІОКэ»єіеЗшЦёХл
	* @param    piValueBuf  КэѕЭЧЄ»»єу»єґжµШЦ·
	* @param    iParamNum   КэѕЭёцКэ
	* @return   void
		* @retval   PARAMETER_EMPTY         ГьБоєуГжОґёъІОКэ 
		* @retval   PARAMETER_CORRECT       ГьБоєуГжЛщёъІОКэёсКЅХэИ·
		* @retval   PARAMETER_HELP          ГьБоєуГжёъБЛ ? єЕ
		* @retval   PARAMETER_ERROR         ГьБоІОКэґнОу
*/
int shell_cmdparam(char * str,int * argc,int argv[])
{
	uint8_t cnt;
	uint8_t value;

	while (' ' == *str) ++str;//Мш№эїХёс
	
	if (*str == 0) //Из№ыГьБоєуГжГ»УРёъІОКэЧЦ·ыКдИлЈ¬·µ»ШїХ
	{
		*argc = 0;
		return PARAMETER_EMPTY;
	}

	if (*str == '?')//Из№ыГьБоєуГжµДКЗОКєЕЈ¬·µ»Шhelp
	{
		*argc = 0;
		return PARAMETER_HELP;
	}

	for (cnt = 0; *str && cnt < 4; ++cnt)//ЧЦ·ыІ»ОЄ Ў®\0' µДК±єт
	{
		argv[cnt] = 0;

		//С­»·°СЧЦ·ыґ®ЧЄОЄКэЧЦЈ¬Ц±µЅЧЦ·ыІ»ОЄ 0 - 9
		for (value = *str - '0';value < 10; value = *(++str) - '0')
		{
			argv[cnt] = argv[cnt] * 10 + value;
		}

		if (*str == '\0') //І»РиТЄЕР¶П \r\n 
		{
			*argc = cnt + 1 ;
			return PARAMETER_CORRECT;
		}
		else
		if (*str != ' ')	//Из№ыІ»КЗ 0 - 9 ¶шЗТІ»КЗїХёсЈ¬ФтКЗґнОуЧЦ·ы
		{
			*argc = 0;
			return PARAMETER_ERROR;
		}
		
		while (' ' == *str) ++str;//Мш№эїХёс,јМРшЕР¶ППВТ»ёцІОКэ

	}
	
	*argc = cnt;
	return PARAMETER_CORRECT;
}





/********************************************************************
	* @author   №ЕГґДю
	* @brief    _shell_register 
	*           ЧўІбТ»ёцГьБоєЕєН¶ФУ¦µДГьБоєЇКэ Ј¬З°ЧєОЄ '_' ±нКѕІ»ЅЁТйЦ±ЅУµчУГґЛєЇКэ
	* @param    cmd_name    ГьБоГы
	* @param    cmd_func        ГьБоГы¶ФУ¦µДЦґРРєЇКэ
	* @param    newcmd      ГьБоїШЦЖїй¶ФУ¦µДЦёХл
	* @return   void
*/
void _shell_register(char * cmd_name, cmd_fn_def cmd_func,struct shell_cmd * newcmd)//ЧўІбГьБо
{
	char * str = cmd_name;
	union uncmd unCmd ;

	uint32_t clen;
	uint32_t fcrc8 = 0;
	uint32_t bcrc8 = 0;
	uint32_t sum = 0;

	for (clen = 0; *str ; ++clen)
	{
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
		++str;
	}

	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;
	unCmd.part.Len = clen;
	unCmd.part.Sum = sum;
	unCmd.part.FirstChar = *cmd_name;
	
	newcmd->ID = unCmd.ID;
	newcmd->name = cmd_name;
	newcmd->Func = cmd_func;

	shell_insert_cmd(newcmd);//ГьБо¶юІжКчІеИлґЛЅЪµг

	return ;
}



/********************************************************************
	* @author   №ЕГґДю
	* @brief    _shell_register 
	*           ЧўІбТ»ёцГьБоєЕєН¶ФУ¦µДГьБоєЇКэЈ¬З°ЧєОЄ '_' ±нГчІ»ЅЁТйЦ±ЅУµчУГ
	* @param    cmd_name    ГьБоГы
	* @param    Func        ГьБоГы¶ФУ¦µДЦґРРєЇКэ
	* @param    newcmd      ГьБоїШЦЖїй¶ФУ¦µДЦёХл
	* @return   void
*/
void _Shell_RegisterCommand__(char * cmd_name, cmd_fn_def Func,void * cmdbuf)//ЧўІбГьБо
{
	struct shell_cmd * newcmd = (struct shell_cmd *)cmdbuf;
	char * str = cmd_name;
	union uncmd unCmd ;

	uint8_t clen;
	uint8_t fcrc8 = 0;
	uint8_t bcrc8 = 0;
	uint8_t sum = 0;

	for (clen = 0; *str ; ++clen)
	{
		sum += *str;
		fcrc8 = F_CRC8_Table[fcrc8^*str];
		bcrc8 = B_CRC8_Table[bcrc8^*str];
		++str;
	}

	unCmd.part.CRC1 = fcrc8;
	unCmd.part.CRC2 = bcrc8;
	unCmd.part.Len = clen;
	unCmd.part.Sum = sum;
	unCmd.part.FirstChar = *cmd_name;
	
	newcmd->ID = unCmd.ID;
	newcmd->name = cmd_name;
	newcmd->Func = Func;

	shell_insert_cmd(newcmd);//ГьБо¶юІжКчІеИлґЛЅЪµг

	return ;
}


/********************************************************************
	* @author   №ЕГґДю
	* @brief    shell_cmdlist 
	*           ПФКѕЛщУРЧўІбБЛµДГьБо
	* @param    str       ГьБоєуЛщёъІОКэ
	* @return   NULL
*/
void shell_cmdlist(void * arg)
{
	shellcmd_t * CmdNode;
	struct avl_node *node ;
	
	for (node = avl_first(&shell_root); node; node = avl_next(node))//±йАъємєЪКч
	{
		CmdNode = avl_entry(node,struct shell_cmd, cmd_node);
		printk("\r\n\t%s", CmdNode->name);
	}
	
	printk("\r\n%s",shell_input_sign);
}


/********************************************************************
	* @brief shell_clean_screnn їШЦЖМЁЗеЖБ
	* @param void
	* @return NULL
*/
void shell_clean_screnn(void * arg)
{
	printk("\033[2J\033[%d;%dH",0,0);
}



void shell_puts_set(void * arg)
{
	default_puts = current_puts;
}



/**
	* @author   №ЕГґДю
	* @brief    shell_init 
	*           shell іхКј»Ї
	* @param    sign : shell КдИл±кЦѕЈ¬Из shell >
	* @param    puts : shell Д¬ИПКдіцЈ¬ИзґУґ®їЪКдіцЎЈ
	* @return   NULL
*/
void shell_init(char * sign,fnFmtOutDef puts)
{
//	shell_input_sign = sign;
	sprintf(shell_input_sign,sign);
	print_CurrentOut(puts);
	print_DefaultOut(puts);
	
	//ЧўІб»щ±ѕГьБо
	shell_register_command("cmd-list"    ,shell_cmdlist);
	shell_register_command("clear"       ,shell_clean_screnn);
	shell_register_command("debug-info",shell_puts_set);
}



