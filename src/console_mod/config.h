/*
Microrl library config files
Autor: Eugene Samoylov aka Helius (ghelius@gmail.com)
*/
#ifndef _MICRORL_CONFIG_H_
#define _MICRORL_CONFIG_H_

#define MICRORL_LIB_VER "1.5.3"

#define SIZE_MAS_CMD        10

/*********** CONFIG SECTION **************/
/*
Command line length, define cmdline buffer size. Set max number of chars + 1,
because last byte of buffer need to contain '\0' - NULL terminator, and 
not use for storing inputed char.
If user input chars more then it parametrs-1, chars not added to command line.*/
#define _COMMAND_LINE_LEN (1+100)									// for 32 chars

/*
Command token number, define max token it command line, if number of token 
typed in command line exceed this value, then prints message about it and
command line not to be parced and 'execute' callback will not calls.
Token is word separate by white space, for example 3 token line:
"IRin> set mode test" */
#define _COMMAND_TOKEN_NMB 10

/*
Define you prompt string here. You can use colors escape code, for highlight you prompt,
for example this prompt will green color (if you terminal supports color)*/
//#define _PROMPT_DEFAULT "\033[32mIRin >\033[0m "	// green color
#define _PROMPT_DEFAULT "\033[32mS >\033[0m "	// green color
//#define _PROMPT_DEFAULT "IRin > "

/*
Define prompt text (without ESC sequence, only text) prompt length, it needs because if you use
ESC sequence, it's not possible detect only text length*/
//#define _PROMPT_LEN       4

/*Define it, if you wanna use completion functional, also set completion callback in you code,
now if user press TAB calls 'copmlitetion' callback. If you no need it, you can just set 
NULL to callback ptr and do not use it, but for memory saving tune, 
if you are not going to use it - disable this define.*/
#define _USE_COMPLETE

/*Define it, if you wanna use history. It s work's like bash history, and
set stored value to cmdline, if UP and DOWN key pressed. Using history add
memory consuming, depends from _RING_HISTORY_LEN parametr */
#define _USE_HISTORY

/*
History ring buffer length, define static buffer size.
For saving memory, each entered cmdline store to history in ring buffer,
so we can not say, how many line we can store, it depends from cmdline len,
but memory using more effective. We not prefer dinamic memory allocation for
small and embedded devices. Overhead is 2 char on each saved line*/
#define _RING_HISTORY_LEN 64

/*
Enable Handling terminal ESC sequence. If disabling, then cursor arrow, HOME, END will not work,
use Ctrl+A(B,F,P,N,A,E,H,K,U,C) see README, but decrease code memory.*/
#define _USE_ESC_SEQ

/*
Use snprintf from you standard complier library, but it gives some overhead.
If not defined, use my own u16int_to_str variant, it's save about 800 byte of code size
on AVR (avr-gcc build).
Try to build with and without, and compare total code size for tune library.
*/
//#define _USE_LIBC_STDIO

/*
Enable 'interrupt signal' callback, if user press Ctrl+C */
//#define _USE_CTLR_C

/*
Print prompt at 'microrl_init', if enable, prompt will print at startup, 
otherwise first prompt will print after first press Enter in terminal
NOTE!: Enable it, if you call 'microrl_init' after your communication subsystem 
already initialize and ready to print message */
#undef _ENABLE_INIT_PROMPT

/*
New line symbol */
#define _ENDL_LFCR

#if defined(_ENDL_CR)
#define ENDL "\r"
#elif defined(_ENDL_CRLF)
#define ENDL "\r\n"
#elif defined(_ENDL_LF)
#define ENDL "\n"
#elif defined(_ENDL_LFCR)
#define ENDL "\n\r"
#else
#error "You must define new line symbol."
#endif

/********** END CONFIG SECTION ************/


#if _RING_HISTORY_LEN > 256
#error "This history implementation (ring buffer with 1 byte iterator) allow 256 byte buffer size maximum"
#endif


/* define the Key codes */
#define KEY_NUL 0 /**< ^@ Null character */
#define KEY_SOH 1 /**< ^A Start of heading, = console interrupt */
#define KEY_STX 2 /**< ^B Start of text, maintenance mode on HP console */
#define KEY_ETX 3 /**< ^C End of text */
#define KEY_EOT 4 /**< ^D End of transmission, not the same as ETB */
#define KEY_ENQ 5 /**< ^E Enquiry, goes with ACK; old HP flow control */
#define KEY_ACK 6 /**< ^F Acknowledge, clears ENQ logon hand */
#define KEY_BEL 7 /**< ^G Bell, rings the bell... */
#define KEY_BS  8 /**< ^H Backspace, works on HP terminals/computers */
#define KEY_HT  9 /**< ^I Horizontal tab, move to next tab stop */
#define KEY_LF  10  /**< ^J Line Feed */
#define KEY_VT  11  /**< ^K Vertical tab */
#define KEY_FF  12  /**< ^L Form Feed, page eject */
#define KEY_CR  13  /**< ^M Carriage Return*/
#define KEY_SO  14  /**< ^N Shift Out, alternate character set */
#define KEY_SI  15  /**< ^O Shift In, resume defaultn character set */
#define KEY_DLE 16  /**< ^P Data link escape */
#define KEY_DC1 17  /**< ^Q XON, with XOFF to pause listings; "okay to send". */
#define KEY_DC2 18  /**< ^R Device control 2, block-mode flow control */
#define KEY_DC3 19  /**< ^S XOFF, with XON is TERM=18 flow control */
#define KEY_DC4 20  /**< ^T Device control 4 */
#define KEY_NAK 21  /**< ^U Negative acknowledge */
#define KEY_SYN 22  /**< ^V Synchronous idle */
#define KEY_ETB 23  /**< ^W End transmission block, not the same as EOT */
#define KEY_CAN 24  /**< ^X Cancel line, MPE echoes !!! */
#define KEY_EM  25  /**< ^Y End of medium, Control-Y interrupt */
#define KEY_SUB 26  /**< ^Z Substitute */
#define KEY_ESC 27  /**< ^[ Escape, next character is not echoed */
#define KEY_FS  28  /**< ^\ File separator */
#define KEY_GS  29  /**< ^] Group separator */
#define KEY_RS  30  /**< ^^ Record separator, block-mode terminator */
#define KEY_US  31  /**< ^_ Unit separator */

#define KEY_DEL 127 /**< Delete (not a real control character...) */

#define IS_CONTROL_CHAR(x) ((x)<=31)


/*
//color for console
#define END_COLOR           "\033[0m"
#define RED_BEGIN           "\033[31m"
#define GREE_BEGIN          "\033[32m"
#define YELLOW_BEGIN        "\033[33m"
#define BLUE_BEGIN          "\033[34m"
#define PURPUR_BEGIN        "\033[35m"
#define BERUZ_BEGIN         "\033[36m"
#define WHITE_BEGIN         "\033[37m"
*/

#endif
