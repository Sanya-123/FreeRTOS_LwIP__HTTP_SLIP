#ifndef PARAMERTS_H
#define PARAMERTS_H

//единственый файл который будет управлять параметрами

#include "common.h"
#include "lwip/apps/snmp_core.h"

extern struct snmp_mib myMibsParametrs;
void initParam();

#endif // PARAMERTS_H
