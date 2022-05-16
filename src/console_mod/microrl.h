#ifndef _MICRORL_H_
#define _MICRORL_H_

#include "config.h"
#include <stdbool.h>

//#define true  1
//#define false 0


// direction of history navigation
#define _HIST_UP   0
#define _HIST_DOWN 1
// esc seq internal codes
#define _ESC_BRACKET  1
#define _ESC_HOME     2
#define _ESC_END      3

#ifdef _USE_HISTORY
// history struct, contain internal variable
// history store in static ring buffer for memory saving
typedef struct {
	char ring_buf [_RING_HISTORY_LEN];
	int begin;
	int end;
	int cur;
} ring_history_t;
#endif

// microrl struct, contain internal library data
typedef struct {
#ifdef _USE_ESC_SEQ
	char escape_seq;
	char escape;
#endif
#if (defined(_ENDL_CRLF) || defined(_ENDL_LFCR))
	char tmpch;
#endif
#ifdef _USE_HISTORY
	ring_history_t ring_hist;          // history object
#endif
	char * prompt_str;                 // pointer to prompt string
	char cmdline [_COMMAND_LINE_LEN];  // cmdline buffer
	int cmdlen;                        // last position in command line
	int cursor;                        // input cursor
    int (*execute) (void * pThis, int argc, const char * const * argv );            // ptr to 'execute' callback
    char ** (*get_completion) (void * pThis, int argc, const char * const * argv ); // ptr to 'completion' callback
	void (*print) (const char *);                                     // ptr to 'print' callback
#ifdef _USE_CTLR_C
    void (*sigint) (void * pThis);
#endif
    bool terminalHasEcho;               //указатель на то счо у терминала есть свое эхо
    short _PROMPT_LEN;                  //     = 4
} microrl_t;

// init internal data, calls once at start up
void microrl_init (microrl_t * pThis, void (*print)(const char*));

//my
void microlSetPromtStr(microrl_t * pThis, char * prompt);
void microlSetPromtStr(microrl_t * pThis, char * prompt);
void microlSetPromptLen(microrl_t * pThis, short lne);

// set echo mode (true/false), using for disabling echo for password input
// echo mode will enabled after user press Enter.
void microrl_set_echo (microrl_t * pThis, bool echo);

// set pointer to callback complition func, that called when user press 'Tab'
// callback func description:
//   param: argc - argument count, argv - pointer array to token string
//   must return NULL-terminated string, contain complite variant splitted by 'Whitespace'
//   If complite token found, it's must contain only one token to be complitted
//   Empty string if complite not found, and multiple string if there are some token
void microrl_set_complete_callback (microrl_t * pThis, char ** (*get_completion)(void *,int, const char* const*));

// pointer to callback func, that called when user press 'Enter'
// execute func param: argc - argument count, argv - pointer array to token string
void microrl_set_execute_callback (microrl_t * pThis, int (*execute)(void *, int, const char* const*));

// set callback for Ctrl+C terminal signal
#ifdef _USE_CTLR_C
void microrl_set_sigint_callback (microrl_t * pThis, void (*sigintf)(void *));
#endif

// insert char to cmdline (for example call in usart RX interrupt)
void microrl_insert_char (microrl_t * pThis, int ch);

#endif
