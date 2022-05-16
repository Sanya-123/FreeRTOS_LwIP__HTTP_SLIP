#include "theardNetWork.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "netif/slipif.h"
#include "lwip/tcpip.h"
#include <stdbool.h>
//http
#include "lwip/apps/httpd.h"
//snmp
#include "snmp_board.h"

#ifdef TEST_NETWORK
#include "lwip/apps/lwiperf.h"

static void
lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{
  LWIP_UNUSED_ARG(arg);
  LWIP_UNUSED_ARG(local_addr);
  LWIP_UNUSED_ARG(local_port);

  LWIP_PLATFORM_DIAG(("IPERF report: type=%d, remote: %s:%d, total bytes: %"U32_F", duration in ms: %"U32_F", kbits/s: %"U32_F"\n",
    (int)report_type, ipaddr_ntoa(remote_addr), (int)remote_port, bytes_transferred, ms_duration, bandwidth_kbitpsec));
}
#endif

struct netif sl_netif;

void theard_NetWork_Server(void *p)
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

#ifdef TEST_NETWORK
    lwiperf_start_tcp_server_default(lwiperf_report, NULL);
    xprintf("Start test netvork\n");
#endif

#if LWIP_SNMP
    snmp_init_board();
#endif

//    //************************************info************************************
//    TaskStatus_t status;
//    vTaskGetInfo(NULL, &status, pdTRUE, eInvalid);

//    xprintf("+--------------------------------+\n");
//    xprintf("Task name:%s\n", status.pcTaskName);
//    xprintf("Size FREE:%d\n", status.usStackHighWaterMark);
//    xprintf("+--------------------------------+\n");
//    //************************************************************************

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
