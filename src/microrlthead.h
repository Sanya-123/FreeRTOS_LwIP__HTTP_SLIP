#ifndef MICRORLTHEAD_H
#define MICRORLTHEAD_H

#include "common.h"
#include "microrl.h"

void taskMicroRl(void *p);
//void recyveDat(uint8_t data);
void recymeMas(uint8_t *data, uint32_t size);

void setConsolePrompt(char * name, bool admin);


#endif // MICRORLTHEAD_H
