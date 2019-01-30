#include "config.h"
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "microrl.h"
#include "misc.h"
#include <unistd.h>

/*
AVR platform specific implementation routines (for Atmega8, rewrite for your MC)
*/
#define _ARM_DEMO_VER "1.0"

// definition commands word
#define _CMD_HELP   		"help"
#define _CMD_CLEAR  		"clear"
#define _CMD_EBI_RV  		"ebi_rv"
//#define _CMD_READ_TEMP    	"read_temper"
//#define _CMD_SET    		"set_vint"
//#define _CMD_PWR    		"set_power"
//#define _CMD_READ_CLK_W    	"read_vint"
//#define _CMD_READ_U_I_P    	"read_U_I_P"
// arguments for set vint
    #define _V		  	"V"	//all ventilators
    #define _V0		  	"V0"
    #define _V1		  	"V1"
    #define _V2		  	"V2"
    #define _V3		  	"V3"
    #define _AUTO		"A"

#define _NUM_OF_CMD 3
#define _NUM_OF_SETCLEAR_SCMD 5

//available  commands
char * keyworld [] = {_CMD_HELP, _CMD_CLEAR, _CMD_EBI_RV/*, _CMD_SET, _CMD_READ_TEMP,\
        _CMD_PWR, _CMD_READ_CLK_W, _CMD_READ_U_I_P*/};
// 'set' command argements
char * set_clear_key [] = {_V, _V0, _V1, _V2, _V3};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];


void put_char (unsigned char ch);


//*****************************************************************************
void init (void)
{
//	configureUart(SystemCoreClock, 9600);//это есть в main.c
}

////*****************************************************************************
//void put_char (unsigned char ch)
//{
//		while (!( UCSRA & (1<<UDRE)));
//		UDR = (unsigned char) ch;
//}

//*****************************************************************************
//extern void print (const char * str);
void print (void *vpThis, const char * str)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    pThis->print(str);
//    int i = 0;
//    while (str [i] != 0) {
////		writeUartData(str[i++]);
//        putUart6(str[i++]);
//    }
}

//*****************************************************************************
//char get_char (void)
//{
//	while (!(UCSRA & (1<<RXC)));
//	return UDR;
//}

//*****************************************************************************
void print_help (microrl_t *pThis)
{
    print (pThis, "\nUse TAB key for completion\n\rCommand:\n\r");
    print (pThis, "\tclear               	- clear screen\n\r");
    print (pThis, "\tebi_rv               	- do ebi_rv\n\r");
}

//*****************************************************************************


//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (void *vpThis, int argc, const char * const * argv)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    opterr = 0;
    optind = 0;

//    setOutMicroRl__ebi_rv(pThis);
//    main_ebi_rw(argc, argv);    //тупо вызываю периложение
//    return 0;

    int i = 0;
    setOutMicroRl__ebi_rv(pThis);
    main_ebi_rw(argc - i, &(argv[i]));    //тупо вызываю периложение
    return 0;
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) {
            print (pThis, "\n\rmicrorl v");
            print (pThis, MICRORL_LIB_VER);
            print (pThis, " library ARM DEMO v");
            print (pThis, _ARM_DEMO_VER);
            print(pThis, "\n\r");
            print_help (pThis);        // print help
		} else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
            print (pThis, "\033[2J");    // ESC seq for clear entire screen
            print (pThis, "\033[H");     // ESC seq for move cursor at left-top corner
        }  else if(strcmp (argv[i], _CMD_EBI_RV) == 0) {    //apps: EBI_RV
            setOutMicroRl__ebi_rv(pThis);
            main_ebi_rw(argc - i, &(argv[i]));    //тупо вызываю периложение
            break;
        }
        /*else if (strcmp (argv[i], _CMD_READ_TEMP) == 0) {
			uint16_t t0 = readTemper(0);
			uint16_t t1 = readTemper(1);
			xprintf("\nt0=%d;t1=%d\n\r", t0, t1);
		}  else if (strcmp (argv[i], _CMD_READ_CLK_W) == 0) {
			print("\n\r");
			taskGetCLK();
		}  else if (strcmp (argv[i], _CMD_READ_U_I_P) == 0) {
			print("\n\r");
			taskSendDebData();
		}  else if (strcmp (argv[i], _CMD_PWR) == 0) {
			if (++i < argc) {
				uint8_t val = atoi (argv[i]);
				if(val == 1)
				{
					print ("\nPower is on\n\r");
					onPower();
				}
				else if(val == 0)
				{
					print ("\nPower is off\n\r");
					offPower();
				}
				else
					print ("\nval must be only '0' or '1'\n\r");
			} else {
				print ("\nspecify val number, use Tab\n\r");
				return 1;
			}
		}  else if (strcmp (argv[i], _CMD_SET) == 0) {
			if (++i < argc) {
//				int val = strcmp (argv[i-1], _CMD_SET);
//				unsigned char * port = NULL;
				int vint = 0;
//				int pin = 0;
				if (strcmp (argv[i], _V0) == 0) {
//					port = (unsigned char *)&PORTD;
					vint = 0;
				} else if (strcmp (argv[i], _V1) == 0) {
//					port = (unsigned char *)&PORTB;
					vint = 1;
				} else if (strcmp (argv[i], _V2) == 0) {
					vint = 2;
				} else if (strcmp (argv[i], _V3) == 0) {
					vint = 3;
				} else if (strcmp (argv[i], _V) == 0) {
					vint = 4;//all ventilators
				} else {
					print ("only '");
					print (_V0);
					print ("-");
					print (_V3);
					print ("' support\n\r");
					return 1;
				}
				if (++i < argc) {
					if(strcmp (argv[i], _AUTO) == 0)
					{
						RTOS_SetTask(taskTemp, 750, 750);
						print ("\nVentilator in automatical mode\n\r");
					}
					else
					{
						RTOS_DeleteTask(taskTemp);
						uint8_t val = atoi (argv[i]);
						setTimer(vint, 100 - val);
						print ("\nVentilator is set\n\r");
					}
					return 0;
				} else {
					print ("\nspecify val number, use Tab\n\r");
					return 1;
				}
			} else {
					print ("\nspecify vint, use Tab\n\r");
				return 1;
			}
        }*/ else {
            print (pThis, "\ncommand: '");
            print (pThis, (char*)argv[i]);
            print (pThis, "' Not found.\n\r");
        }
        i++;
    }
	return 0;
}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char ** complet (void *vpThis, int argc, const char * const * argv)
{
    microrl_t *pThis = (microrl_t *)vpThis;
	int j = 0;

	compl_world [0] = NULL;

	// if there is token in cmdline
	if (argc == 1) {
		// get last entered token
		char * bit = (char*)argv [argc-1];
		// iterate through our available token and match it
		for (int i = 0; i < _NUM_OF_CMD; i++) {
			// if token is matched (text is part of our token starting from 0 char)
			if (strstr(keyworld [i], bit) == keyworld [i]) {
				// add it to completion set
				compl_world [j++] = keyworld [i];
			}
		}
    }	/*else if ((argc > 1) && ((strcmp (argv[0], _CMD_SET)==0) ||
													 (strcmp (argv[0], _CMD_READ_TEMP)==0))) { // if command needs subcommands
		// iterate through subcommand
		for (int i = 0; i < _NUM_OF_SETCLEAR_SCMD; i++) {
			if (strstr (set_clear_key [i], argv [argc-1]) == set_clear_key [i]) {
				compl_world [j++] = set_clear_key [i];
			}
		}
    }*/ else { // if there is no token in cmdline, just print all available token
		for (; j < _NUM_OF_CMD; j++) {
			compl_world[j] = keyworld [j];
		}
	}

	// note! last ptr in array always must be NULL!!!
	compl_world [j] = NULL;
	// return set of variants
	return compl_world;
}
#endif

//*****************************************************************************
void sigint (void * vpThis)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    print (pThis, "^C catched!\n\r");
}


