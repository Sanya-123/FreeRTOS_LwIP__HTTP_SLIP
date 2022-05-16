#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
//#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>

#include "ebi_rw.h"
#include "xprintf.h"

//#include <getopt.h>
#include "getopt.h"

//#define OPTPARSE_IMPLEMENTATION
//#define OPTPARSE_API static
//#include "optparse.h"

microrl_t * outPutData;

void setOutMicroRl__ebi_rv(microrl_t * pThis)
{
    outPutData = pThis;
    //printf => outPutData->print
}

void outputFunction(unsigned char c)
{
    char ch[2] = {c, 0};
    outPutData->print(ch);
}

#define VERSION_NUMBER 3

#if defined(EBI_RW)
    #define REGION_SIZE             0x04000000
    #define MAXIMUM_ESPI_DATA_SIZE  64
    #define MAX_ADDRESS (REGION_SIZE - 1)
#elif defined(SPI_RW)
    #define MAX_ADDRESS             255
    #define MAXIMUM_ESPI_DATA_SIZE  4
#else
    #error Use SPI_RW or EBI_RW defines
#endif

typedef enum
{
    IOCTL_EBI_RW = 0xB0,
    IOCTL_SPI_RW = 0xB1,
} ESPI_IOCTL_Commands;

typedef struct
{
    uint32_t address;
    uint8_t spi;
    bool    isWrite;
    size_t datasize;
    uint8_t data[MAXIMUM_ESPI_DATA_SIZE];
} ESPI_RW_Package;

struct Options
{
    bool verbose;
    bool data_signed;
    uint32_t address_offset;
    uint8_t data_base;
    uint8_t data_bytes;
} options;

void print_help_ebi_rw(const char *program_name)
{
    xfprintf(outputFunction,"Version 0.%d Build %s %s\n", VERSION_NUMBER, __DATE__, __TIME__);
#ifdef GIT_REPO_VERSION
    xfprintf(outputFunction,"Git version: %s\n", GIT_REPO_VERSION);
#endif

    xfprintf(outputFunction,"\nUsage:\n");
    xfprintf(outputFunction,"    %s [-hds] [-o offset] [-n bytes] [-b base] ", program_name);
#if defined(SPI_RW)
    xfprintf(outputFunction,"fpga register [data]\n");
#elif defined(EBI_RW)
    xfprintf(outputFunction,"address datasize [data]\n");
#endif

    xfprintf(outputFunction,"\nOptions:\n");
    xfprintf(outputFunction,"    -h  help, print this help\n");
    xfprintf(outputFunction,"    -d  debug, enable debug mode to print debugging messages\n");
    xfprintf(outputFunction,"    -s  signed, set signed format for output data (only for dec base)\n");
    xfprintf(outputFunction,"    -o  offset, offset for address\n");
    xfprintf(outputFunction,"    -n  bytes (1|2|4), set size to data (0xAA 0xAABB 0xAABBCCDD). Default-1.\n");
    xfprintf(outputFunction,"    -b  base (2|10|16), set output data format (bin|dec|hex). Default-10.\n");

    xfprintf(outputFunction,"\nSupported prefixes for input data:\n");
    xfprintf(outputFunction,"    hex:  0x x 0X X 0h h 0H H\n");
    xfprintf(outputFunction,"    bin:  0b b 0B B\n");

    xfprintf(outputFunction,"\nExamples:\n");
#if defined(EBI_RW)
    xfprintf(outputFunction,"    %s 0xAA 1            - read  1 byte\n", program_name);
    xfprintf(outputFunction,"    %s 0xAA 1 0xBB       - write 1 byte\n", program_name);
    xfprintf(outputFunction,"    %s 0xAA 2 0xBB 0xCC  - write 2 byte\n", program_name);
    xfprintf(outputFunction,"    %s 0xAA 2 0xBBCC -n2 - write 2 byte\n", program_name);
#elif defined(SPI_RW)
    xfprintf(outputFunction,"    %s 0xAA 1 1            - read 1 byte,  fpga 1\n", program_name);
    xfprintf(outputFunction,"    %s 0xAA 0 1 0xBB       - write 1 byte, fpga 1\n", program_name);
    xfprintf(outputFunction,"    %s 0xAA 4 2 0xBB 0xCC  - write 2 byte, fpga 4\n", program_name);
    xfprintf(outputFunction,"    %s -n2 0xAA 4 2 0xBBCC - write 2 byte, fpga 4\n", program_name);
#endif
}

unsigned long parse_number(char *str)
{
    // Функция преобразует строку с число.
    // Допустимые форматы:
    //   hex:  0x x 0X X 0h h 0H H
    //   bin:  0b b 0B B
    //   dec:  other

    char *prefix, *data_string;
    int base;

    if ((prefix = strrchr(str, 'x')) || (prefix = strrchr(str, 'X')) ||
            (prefix = strrchr(str, 'h')) || (prefix = strrchr(str, 'H'))) // По префиксу определяем:
    {
        base = 16;                                                      //  формат данных
        data_string = prefix + 1;                                       //  указатель на начало этих данных
    }
    else if ((prefix = strrchr(str, 'b')) || (prefix = strrchr(str, 'B')))
    {
        base = 2;
        data_string = prefix + 1;
    }
    else
    {
        base = 10;
        data_string = str;
    }

    errno = 0;
    char *ptr = 0;
    unsigned long data_num = strtoul(data_string, &ptr, base);  // Обрезанную строку разбираем с помощью strtol, разумееется,
    //                                                          // проверяем результат. Тут перечислены условия проверок:
    if ((ptr <= str) || (*ptr != '\0') || (errno == ERANGE))    //  http://knzsoft.blogspot.ru/2015/04/strtol-strtoll.html
    {
        xfprintf(outputFunction,"Error: unknow data formaton in argument \"%s\"\n", str);
        exit(EXIT_FAILURE);//TODO
    }

    return data_num;
}

uint32_t parse_data(char *str)
{
    unsigned long data = parse_number(str);

    if (options.data_bytes == 4 && data > UINT32_MAX)     // Проверяем на переполнение
    {
        xfprintf(outputFunction,"Error: argument \"%s\" > UINT32_MAX\n", str);
        exit(EXIT_FAILURE);
    }
    else if (options.data_bytes == 2 && data > UINT16_MAX)
    {
        xfprintf(outputFunction,"Error: argument \"%s\" > UINT16_MAX\n", str);
        exit(EXIT_FAILURE);
    }
    else if (options.data_bytes == 1 && data > UINT8_MAX)
    {
        xfprintf(outputFunction,"Error: argument \"%s\" > UINT8_MAX\n", str);
        exit(EXIT_FAILURE);
    }

    return data;
}

uint32_t parse_address(char *str)
{
    unsigned long address = parse_number(str);

    if (address > MAX_ADDRESS)
    {
        xfprintf(outputFunction,"Error: address can not be over 0x%X\n", MAX_ADDRESS);
        exit(EXIT_FAILURE);
    }

    return address;
}

void swap(uint8_t *data, uint32_t size)
{
    for (uint32_t i = 0; i < (size / 2); i++)
    {
        uint8_t tmp = data[i];
        data[i] = data[size - i - 1];
        data[size - i - 1] = tmp;
    }
}

int main_ebi_rw(int argc, char **argv)
{
    char *program_name = strrchr(argv[0], '/');
    if (program_name == NULL)
    {
        program_name = argv[0];
    }
    else
    {
        program_name++; // remove '/'
    }

    options.data_base = 10;
    options.data_bytes = 1;
    options.address_offset = 0;
    options.verbose = false;
    options.data_signed = false;


    int opt = 0;
//    struct optparse optionsOpt;
//    optparse_init(&optionsOpt, argv);
//    optionsOpt.optind = 1;
//    while ((opt = optparse(&optionsOpt, "hdso:b:n:")) != -1)  // Разбираем ключи
    optind = 0;
    while ((opt = getopt(argc, argv, "hdso:b:n:")) != EOF)  // Разбираем ключи
    {
        switch (opt)
        {
            case 'o':                                       // смещение для адреса
                options.address_offset = parse_address(/*optionsOpt.*/optarg);
                break;
            case 'b':                                       // формат выводимых данных (bin dec hex)
                options.data_base = atoi(/*optionsOpt.*/optarg);
                if (options.data_base != 2 &&  options.data_base != 10 && options.data_base != 16)
                {
                    print_help_ebi_rw(program_name);
//                    exit(EXIT_FAILURE);
                    return 0;
                }
                break;
            case 'n':                                       // размерность выводимых данных (0xFF 0xFFFF 0xFFFFFFFF)
                options.data_bytes = atoi(/*optionsOpt.*/optarg);
                if (options.data_bytes != 1 && options.data_bytes != 2 && options.data_bytes != 4)
                {
                    print_help_ebi_rw(program_name);
//                    exit(EXIT_FAILURE);
                    return 0;
                }
                break;
            case 's':                                       // читаемые данные будут читаться знаковыми
                options.data_signed = true;
                break;
            case 'd':                                       // отладочные сообщения
                options.verbose = true;
                break;
            case '?':                                       // вывод справки
            case 'h':
            default:
                print_help_ebi_rw(program_name);
//                exit(EXIT_FAILURE);
                return 0;
        };
    };


    argv += /*optionsOpt.*/optind;
    argc -= /*optionsOpt.*/optind;                                     // Из аргументов исключаем ключи, после чего должны
    //                                                  // остаться как минимум - адрес и размер данных
    if (argc < 2 || argc > (2 + MAXIMUM_ESPI_DATA_SIZE))// или номер fpga и адрес
    {
        xfprintf(outputFunction,"Error: data arguments too much or too few\n");
//        exit(EXIT_FAILURE);
        return 0;
    }

    // --------------- Теперь  начинаем разбирать данные ---------------

    ESPI_RW_Package package = {0};
    int i = 0;
#if defined(EBI_RW)
    package.address = parse_address(argv[i++]);   // сохраняем адрес
    package.datasize = parse_address(argv[i++]);  // сохраняем размер данных
#elif defined(SPI_RW)
    package.spi = parse_address(argv[i++]);     // сохраняем адрес fpga
    package.address = parse_address(argv[i++]);  // сохраняем адрес регистра
    package.datasize = 4;
#endif
    argv += i;                              // и 2 этих аргумента убираем
    argc -= i;                              // из исходных ранных

    if (package.datasize > MAXIMUM_ESPI_DATA_SIZE) // Проверяем входные данные на валидность
    {
        xfprintf(outputFunction,"Error: datasize argument over MAXIMUM_ESPI_DATA_SIZE\n");
//        exit(EXIT_FAILURE);
        return 0;
    }
    if (package.datasize == 0)
    {
        xfprintf(outputFunction,"Error: datasize can not be 0\n");
//        exit(EXIT_FAILURE);
        return 0;
    }
    if (package.datasize % options.data_bytes)
    {
        xfprintf(outputFunction,"Error: datasize not divided on bytesize (-n)\n");
//        exit(EXIT_FAILURE);
        return 0;
    }
#if defined(SPI_RW)
    if (package.spi < 1 || package.spi > 11)  // 1 modem + 2 dos + 8 ppm
    {
        xfprintf(outputFunction,"Error: fpga number %d is wrong, should be 1...11\n", package.spi);
//        exit(EXIT_FAILURE);
        return 0;
    }

    package.spi--; // переход от номера fpga к номеру spi
#endif

    if (argc == 0)                  // Раз есть только адрес и размер данных - это чтение.
    {
        package.isWrite = false;
    }
    else // (argc > 0)              // А это - запись.
    {
        package.isWrite = true;

        uint32_t real_datasize = options.data_bytes * argc;
        uint32_t index_offset = package.datasize - real_datasize;

#if defined(EBI_RW)
        if (real_datasize != package.datasize)
#elif defined(SPI_RW)
        if (real_datasize > package.datasize)
#endif
        {
            xfprintf(outputFunction,"Error: datasize not equal to the entered data\n");
//            exit(EXIT_FAILURE);
            return 0;
        }

        for (uint32_t i = 0; i < real_datasize; i++)  // раскладываем данные по байтам
        {
            {
                uint32_t tmp_data = parse_data(argv[i / options.data_bytes]);
                uint8_t bytes = options.data_bytes - (i % options.data_bytes) - 1;
                package.data[i + index_offset] = tmp_data >> (8 * bytes);
            }
        }
    }

    package.address += options.address_offset; // Добавляем смещение

#if defined(EBI_RW)
    if ((package.address + package.datasize - 1) > MAX_ADDRESS)
    {
        xfprintf(outputFunction,"Error: offset+address+datasize can not be over 0x%X\n", MAX_ADDRESS);
//        exit(EXIT_FAILURE);
        return 0;
    }
#elif defined(SPI_RW)
    if (package.address > MAX_ADDRESS)
    {
        xfprintf(outputFunction,"Error: offset+address can not be over 0x%X\n", MAX_ADDRESS);
//        exit(EXIT_FAILURE);
        return 0;
    }
#endif

    // --------------- Данные готовы, отправляем ---------------

    swap(package.data, package.datasize);

    if (options.verbose)            // Немного отладочной информации
    {
        if (package.isWrite)
        {
            xfprintf(outputFunction,"WRITE:\n");
        }
        else
        {
            xfprintf(outputFunction,"READ:\n");
        }
#if defined(SPI_RW)
        xfprintf(outputFunction,"  .spi:       %d\n", package.spi);
#endif
        xfprintf(outputFunction,"  .address:  0x%08X\n", package.address);
        xfprintf(outputFunction,"  .datasize: 0x%08X\n", (uint32_t)package.datasize);
        if (package.isWrite)
        {
            for (uint32_t i = 0; i < package.datasize; i++)
            {
                xfprintf(outputFunction,"    .data[%2d]: 0x%02X\n", i, package.data[i]);
            }
        }
    }
#if 0
    int file_desc = open("/dev/espi", O_RDWR);
    if (file_desc < 0)
    {
        xfprintf(outputFunction,"Can't open device file /dev/espi\n");
        exit(EXIT_FAILURE);
    }

#if defined(EBI_RW)
    int state = ioctl(file_desc, IOCTL_EBI_RW, &package);
#elif defined(SPI_RW)
    int state = ioctl(file_desc, IOCTL_SPI_RW, &package);
#endif

    close(file_desc);
#endif

    int state = 0;

    if (state)
    {
        if (errno == EACCES)
        {
            xfprintf(outputFunction,"Acess error\n");
        }
        else if (errno == ETIME)
        {
            xfprintf(outputFunction,"Timeout error\n");
        }
        else
        {
            xfprintf(outputFunction,"Unknow error %d\n", errno);
        }
//        exit(EXIT_FAILURE);
        return 0;
    }

    if (options.verbose)            // Немного отладочной информации
    {
        if (package.isWrite == false)
        {
            for (uint32_t i = 0; i < package.datasize; i++)
            {
                xfprintf(outputFunction,"    .data[%2d]: 0x%02X\n", i, package.data[i]);
            }
        }
    }

    swap(package.data, package.datasize);

    if (package.isWrite == false)            // Печатаем полученные данные
    {
        uint32_t tmp_data = 0;
        for (uint32_t i = 0; i < package.datasize; i++)  // раскладываем данные
        {
            tmp_data <<= 8;
            tmp_data |= package.data[i];

            if ((i + 1) % options.data_bytes  == 0)
            {
                if (options.data_base == 2)         // добавляем префикс
                {
                    xfprintf(outputFunction,"b");
                }
                else if (options.data_base == 16)
                {
                    xfprintf(outputFunction,"0x");
                }

                if (options.data_base == 2)         // печатаем 2
                {
                    for (int i = 8 * options.data_bytes - 1; i >= 0; i--)
                    {
//                        putchar((tmp_data >> i) & 1 ? '1' : '0');
                        xfprintf(outputFunction,"%c", (tmp_data >> i) & 1 ? '1' : '0');
                    }
                }
                else if (options.data_base == 16)   // печатаем 16
                {
                    if (options.data_bytes == 1)
                    {
                        xfprintf(outputFunction,"%02X", tmp_data);
                    }
                    else if (options.data_bytes == 2)
                    {
                        xfprintf(outputFunction,"%04X", tmp_data);
                    }
                    else if (options.data_bytes == 4)
                    {
                        xfprintf(outputFunction,"%08X", tmp_data);
                    }
                }
                else if (options.data_base == 10)   // печатаем 10
                {
                    if (options.data_signed && options.data_bytes == 1)
                    {
                        xfprintf(outputFunction,"%d", (int8_t)tmp_data);
                    }
                    else if (options.data_signed && options.data_bytes == 2)
                    {
                        xfprintf(outputFunction,"%d", (int16_t)tmp_data);
                    }
                    else if (options.data_signed && options.data_bytes == 4)
                    {
                        xfprintf(outputFunction,"%d", (int32_t)tmp_data);
                    }
                    else
                    {
                        xfprintf(outputFunction,"%u", tmp_data);
                    }
                }

                xfprintf(outputFunction,"\n");
                tmp_data = 0;
            }
        }
    }
    xfprintf(outputFunction, "OK\n");
    return 0;
//    exit(EXIT_SUCCESS);
}
