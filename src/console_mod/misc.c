#include "config.h"
#include <string.h>
#include <stdlib.h>
#include "uart.h"
#include "microrl.h"
#include "misc.h"
//#include <getopt.h>

/*
AVR platform specific implementation routines (for Atmega8, rewrite for your MC)
*/
#define _ARM_DEMO_VER "1.2"

typedef struct{
    char *cmdName;
    int (*execCmd) (void * pThis, int argc, const char * const * argv ); //указатель на вызываемую функцию
}CmdStruct;

CmdStruct masCMD[SIZE_MAS_CMD];
static uint16_t sizeCmd = 0;

// definition commands word
#define _CMD_HELP   		"help"
#define _CMD_CLEAR  		"clear"
//#define _CMD_TASK_I  		"aboutTask"
//#define _CMD_TN_AB  		"TelNetAbonentCon"
//#define _CMD_CPU_IDLE  		"CPU_Idle"
//#define _CMD_COU_GR  		"CPU_Gra"
//#define _CMD_EBI_RV  		"ebi_rv"

//#define _NUM_OF_CMD 7

//available  commands
char *keyworld [SIZE_MAS_CMD] = {0};

// array for comletion
char *compl_world [SIZE_MAS_CMD + 1];


//*****************************************************************************
//extern void print (const char * str);
void print(void *vpThis, const char *str)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    pThis->print(str);
}

//*****************************************************************************
int print_help(void *vpThis, int argc, const char *const *argv)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    (void)argc;
    (void)argv;

    print(pThis, "\n\rmicrorl v");
    print(pThis, MICRORL_LIB_VER);
    print(pThis, " library ARM DEMO v");
    print(pThis, _ARM_DEMO_VER);
    print(pThis, "\n\r");

//    const char mas150MHz[] ="  ___   ______   _______        __        __       _     _        \r\n"
//                            " /   | |  ____| |  ___  |      /  \\      /  \\     | |   | |  ____ \r\n"
//                            "/_/| | | |____  | |   | |     / /\\ \\    / /\\ \\    | |___| | |_   |\r\n"
//                            "   | | |____  | | |   | |    / /  \\ \\  / /  \\ \\   |  ___  |   / / \r\n"
//                            "   | |  ____| | | |_ _| |   / /    \\ \\/ /    \\ \\  | |   | |  / /_ \r\n"
//                            "   |_| |______| |_______|  /_/      \\__/      \\_\\ |_|   |_| /____|\r\n"
//            ;
    const char mas150MHz[] ="\t   ___   ______   _______    ___    ___   _     _        \r\n"\
                            "\t  /   | |  ____| |  ___  |  |   \\  /   | | |   | |  ____ \r\n"\
                            "\t /_/| | | |____  | |   | |  | |\\ \\/ /| | | |___| | |_   |\r\n"\
                            "\t    | | |____  | | |   | |  | | \\__/ | | |  ___  |   / / \r\n"\
                            "\t    | |  ____| | | |_ _| |  | |      | | | |   | |  / /_ \r\n"\
                            "\t    |_| |______| |_______|  |_|      |_| |_|   |_| /____|\r\n"\
            ;
    print(pThis, mas150MHz);
    print(pThis, "\nUse TAB key for completion\n\rCommand:\n\r");
    print(pThis, "\tclear               	- clear screen\n\r");
    print(pThis, "\taboutTask               - print about tasks\n\r");
    print(pThis, "\tTelNetAbonentCon        - print abonent size\n\r");
    print(pThis, "\tCPU_Idle                - print CPU_IDLE\n\r");
    print(pThis, "\tCPU_Gra                 - print aGraph CPU\n\r");
    print(pThis, "\tebi_rv               	- do ebi_rv\n\r");
    return 0;
}

int cmdClear(void *vpThis, int argc, const char *const *argv)
{
    (void)argc;
    (void)argv;
    microrl_t *pThis = (microrl_t *)vpThis;
    print(pThis, "\033[2J");     // ESC seq for clear entire screen
    print(pThis, "\033[H");      // ESC seq for move cursor at left-top corner
    return 0;
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute(void *vpThis, int argc, const char *const *argv)
{
    microrl_t *pThis = (microrl_t *)vpThis;
#ifdef GETOPT_H
    opterr = 0;
    optind = 0;
#endif

    if(sizeCmd == 0)//если не было инициализации
        initDefoultCMD();

    // just iterate through argv word and compare it with your commands
    //**standart comand
    for(int i = 0; i < sizeCmd; i++)//поиск команд
    {
        if (strcmp(argv[0], masCMD[i].cmdName) == 0)//когда команда найдена
        {
            return masCMD[i].execCmd(vpThis, argc, argv);
        }
    }
    //если не получиорсь найти команду
    print(pThis, "\ncommand: '");
    print(pThis, (char *)argv[0]);
    print(pThis, "' Not found.\n\r");
    return -1;

}
////old version execute
//int execute (void *vpThis, int argc, const char * const * argv)
//{
//    microrl_t *pThis = (microrl_t *)vpThis;
//    opterr = 0;
//    optind = 0;

////    setOutMicroRl__ebi_rv(pThis);
////    main_ebi_rw(argc, argv);    //тупо вызываю периложение
////    return 0;

//    int i = 0;
//    setOutMicroRl__ebi_rv(pThis);
//    main_ebi_rw(argc - i, &(argv[i]));    //тупо вызываю периложение
//    return 0;
//	// just iterate through argv word and compare it with your commands
//	while (i < argc) {
//		if (strcmp (argv[i], _CMD_HELP) == 0) {
//            print (pThis, "\n\rmicrorl v");
//            print (pThis, MICRORL_LIB_VER);
//            print (pThis, " library ARM DEMO v");
//            print (pThis, _ARM_DEMO_VER);
//            print(pThis, "\n\r");
//            print_help (pThis);        // print help
//		} else if (strcmp (argv[i], _CMD_CLEAR) == 0) {
//            print (pThis, "\033[2J");    // ESC seq for clear entire screen
//            print (pThis, "\033[H");     // ESC seq for move cursor at left-top corner
//        }  else if(strcmp (argv[i], _CMD_EBI_RV) == 0) {    //apps: EBI_RV
//            setOutMicroRl__ebi_rv(pThis);
//            main_ebi_rw(argc - i, &(argv[i]));    //тупо вызываю периложение
//            break;
//        } else {
//            print (pThis, "\ncommand: '");
//            print (pThis, (char*)argv[i]);
//            print (pThis, "' Not found.\n\r");
//        }
//        i++;
//    }
//	return 0;
//}

#ifdef _USE_COMPLETE
//*****************************************************************************
// completion callback for microrl library
char **complet(void *vpThis, int argc, const char *const *argv)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    (void)pThis;
    int j = 0;

    compl_world [0] = NULL;

    // if there is token in cmdline
//    if (argc == 1)
    if (argc >= 1)
    {
        // get last entered token
        char *bit = (char *)argv [argc - 1];
        // iterate through our available token and match it
        for (int i = 0; i < sizeCmd; i++)
        {
            // if token is matched (text is part of our token starting from 0 char)
            if (strstr(keyworld [i], bit) == keyworld [i])
            {
                // add it to completion set
                compl_world [j++] = keyworld [i];
            }
        }
    } else   // if there is no token in cmdline, just print all available token
    {
        for (; j < sizeCmd; j++)
        {
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
void sigint(void *vpThis)
{
    microrl_t *pThis = (microrl_t *)vpThis;
    print(pThis, "^C catched!\n\r");
}

void addCmd(const char *name, int (*execCmd) (void * pThis, int argc, const char * const * argv ))//добавления команд
{
    if(sizeCmd == SIZE_MAS_CMD)//предел команд
        return;
    masCMD[sizeCmd].cmdName = (char*)name;
    masCMD[sizeCmd].execCmd = execCmd;
    keyworld[sizeCmd++] = (char*)name;
}

void initDefoultCMD()
{
    addCmd(_CMD_HELP, print_help);//дефолтные команды
    addCmd(_CMD_CLEAR, cmdClear);
}


