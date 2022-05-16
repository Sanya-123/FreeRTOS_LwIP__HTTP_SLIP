#ifndef _THEARD_NETWORK_H_
#define _THEARD_NETWORK_H_

#include "common.h"

#define IP_ADDRES_A3        192
#define IP_ADDRES_A2        168
#define IP_ADDRES_A1        10
#define IP_ADDRES_A0        1

#define IP_GW_A3            192
#define IP_GW_A2            168
#define IP_GW_A1            10
#define IP_GW_A0            2

#define IP_NETMASK_A3       255
#define IP_NETMASK_A2       255
#define IP_NETMASK_A1       255
#define IP_NETMASK_A0       0

//SLPI
//IP
//TCP
//HTTP;Ipref;SNMP
void theard_NetWork_Server(void *p);

#endif // _THEARD_NETWORK_H_
