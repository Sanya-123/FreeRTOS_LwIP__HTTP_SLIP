
#ifndef __SHELL_H__
#define __SHELL_H__

// ТФПВОЄ shell ЛщТААµµД»щ±ѕїв
//#include "rbtree.h" //ГьБоЛчТэУГємєЪКчЅшРРІйХТЖҐЕд
#include "avltree.h"//ГьБоЛчТэУГavlКчЅшРРІйХТЖҐЕд
#include "ustdio.h"


//#define     DEFAULT_INPUTSIGN       "shell>"
#define     DEFAULT_INPUTSIGN       "\033[32mS >\033[0m "

#define     KEYCODE_CTRL_C            0x03
#define     KEYCODE_NEWLINE           0x0A
#define     KEYCODE_ENTER             0x0D   //јьЕМµД»Шіµјь
#define     KEYCODE_BACKSPACE         0x08   //јьЕМµД»ШНЛјь
#define     KEYCODE_ESC               0x1b
#define     KEYCODE_TAB               '\t'   //јьЕМµДtabјь



enum INPUT_PARAMETER
{
	PARAMETER_EMPTY,
	PARAMETER_CORRECT,
	PARAMETER_HELP,
	PARAMETER_ERROR
};



/*
-----------------------------------------------------------------------
	µчУГєк shell_register_command(pstr,pfunc) ЧўІбГьБо
	ЧўІбТ»ёцГьБоєЕµДН¬К±»бРВЅЁТ»ёцУлГьБо¶ФУ¦µДїШЦЖїй
	ФЪ shell ЧўІбµДєЇКэАаРННіТ»ОЄ void(*CmdFuncDef)(void * arg); 
	arg ОЄїШЦЖМЁКдИлГьБоєуЛщёъµДІОКэКдИл
-----------------------------------------------------------------------
*/
#define   shell_register_command(pstr,pfunc)\
	do{\
		static struct shell_cmd st##pfunc = {0};\
		_shell_register(pstr,pfunc,&st##pfunc);\
	}while(0)


#define COMMANDLINE_MAX_LEN    36  //ГьБоґшЙПІОКэµДЧЦ·ыґ®КдИлЧоі¤јЗВјі¤¶И
#define COMMANDLINE_MAX_RECORD 4      //їШЦЖМЁјЗВјМхДїКэ


//#define vShell_InitPrint(fn) do{print_CurrentOut(fn);print_DefaultOut(fn);}while(0)

#define SHELL_CMD_LEN(pCommand)  (((pCommand)->ID >> 21) & 0x001F)



typedef void (*cmd_fn_def)(void * arg);


typedef struct shell_cmd
{
	uint32_t	  ID;	 //ГьБо±кК¶Вл
	char *		  name; //јЗВјГїМхГьБоЧЦ·ыґ®µДДЪґжµШЦ·
	cmd_fn_def	  Func;  //јЗВјГьБоєЇКэЦёХл
	//struct rb_node cmd_node;//ємєЪКчЅЪµг
	struct avl_node cmd_node;//avlКчЅЪµг
}
shellcmd_t;


typedef struct shell_buf 
{
  fnFmtOutDef puts;
  char   * bufmem;
  uint32_t index;
}
shellbuf_t;

#define SHELL_MALLOC(shellbuf,shellputs) \
	do{\
		static char bufmem[COMMANDLINE_MAX_LEN] = {0};\
		(shellbuf)->bufmem = bufmem;    \
		(shellbuf)->index  = 0;         \
		(shellbuf)->puts = shellputs;   \
	}while(0)


extern char  shell_input_sign[];
	
void _shell_register(char * cmdname, cmd_fn_def func,struct shell_cmd * newcmd);//ЧўІбГьБо

void shell_input(struct shell_buf * ,char * ,uint8_t );

int  shell_cmdparam  (char * argStr,int * argc,int argv[]);

void shell_init(char * sign,fnFmtOutDef default_print);

#endif

