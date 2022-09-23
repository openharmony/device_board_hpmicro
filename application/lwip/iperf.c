/*
 * Copyright (c) 2022 HPMicro.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <ohos_init.h>
#include <stdio.h>
#include <los_task.h>
#include <stdint.h>
#include <log.h>
#include <lwip/netif.h>
#include <lwip/apps/lwiperf.h>
#include <lwip/netif.h>
#include <lwip/ip_addr.h>
#include <lwip/ip4_addr.h>

#define LOG_TAG "LWIPERF"


static void lwiperf_report(void *arg, enum lwiperf_report_type report_type,
  const ip_addr_t* local_addr, u16_t local_port, const ip_addr_t* remote_addr, u16_t remote_port,
  u32_t bytes_transferred, u32_t ms_duration, u32_t bandwidth_kbitpsec)
{

}


static void IperfServerTest(void)
{
    printf("IperfServerTest");
    
    struct netif* netif;
    char name[3] = {0};
    NETIF_FOREACH(netif) {
        netif_index_to_name(netif_get_index(netif), name);
        if (netif_is_up(netif)) {
            printf("[%s]: up\n", name);
        } else {
            printf("[%s]: down\n", name);
        }

        const ip4_addr_t *addr = netif_ip4_addr(netif);
        printf("[%s]: ip: %u.%u.%u.%u\n", 
                                        name,
                                        ip4_addr1(addr), 
                                        ip4_addr2(addr),
                                        ip4_addr3(addr),
                                        ip4_addr4(addr));
        addr = netif_ip4_netmask(netif);
        printf("[%s]: netmask: %u.%u.%u.%u\n", 
                                        name,
                                        ip4_addr1(addr), 
                                        ip4_addr2(addr),
                                        ip4_addr3(addr),
                                        ip4_addr4(addr));
        addr = netif_ip4_gw(netif);
        printf("[%s]: gw: %u.%u.%u.%u\n", 
                                        name,
                                        ip4_addr1(addr), 
                                        ip4_addr2(addr),
                                        ip4_addr3(addr),
                                        ip4_addr4(addr));

        lwiperf_start_tcp_server(netif_ip_addr4(netif), LWIPERF_TCP_PORT_DEFAULT,
                         lwiperf_report, NULL);
    }
}

APP_FEATURE_INIT(IperfServerTest);