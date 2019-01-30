#include "theardHTTP.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/opt.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/ip.h"
#include "lwip/netif.h"
#include "netif/slipif.h"
#include "lwip/pbuf.h"
#include "lwip/tcpip.h"
#include <stdbool.h>
//http
#include "lwip/apps/httpd.h"

struct netif sl_netif;

void theard_HTTP_Server(void *p)
{
    (void)p;
    // Matches the number in sio_open()
    char int_no = 2;

    ip_addr_t ipaddr;
    ip_addr_t netmask;
    ip_addr_t gw;

    tcpip_init(NULL, NULL);

    // Configure the SLIP interface
    xprintf("Starting SLIP\n");
    IP4_ADDR(&ipaddr, IP_ADDRES_A3, IP_ADDRES_A2, IP_ADDRES_A1, IP_ADDRES_A0);
    IP4_ADDR(&netmask, IP_NETMASK_A3, IP_NETMASK_A2, IP_NETMASK_A1, IP_NETMASK_A0);
    IP4_ADDR(&gw, IP_GW_A3, IP_GW_A2, IP_GW_A1, IP_GW_A0);
//    netif_add(&sl_netif, &ipaddr, &netmask, &gw, &int_no, slipif_init, ip_input);
    netif_add(&sl_netif, &ipaddr, &netmask, &gw, &int_no, slipif_init, tcpip_input);
    netif_set_default(&sl_netif);
    netif_set_up(&sl_netif);
    netif_set_link_up(&sl_netif);

    //HTTP
    xprintf("Start HTTP\n");
    httpd_init();

    //************************************info************************************
    TaskStatus_t status;
    vTaskGetInfo(NULL, &status, pdTRUE, eInvalid);

    xprintf("+--------------------------------+\n");
    xprintf("Task name:%s\n", status.pcTaskName);
    xprintf("Size FREE:%d\n", status.usStackHighWaterMark);
    xprintf("+--------------------------------+\n");
    //************************************************************************

#if !NO_SYS
    vTaskDelete(NULL);
#endif

    //TODO сделать 1 общий таск
    while (1)
    {
//        slipif_process_rxqueue(&sl_netif);
#if NO_SYS
        slipif_poll(&sl_netif);
#endif
    }
}
