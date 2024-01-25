/*
 * Copyright (c) 2022 HPMicro
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
#ifndef HPM_LWIP_H
#define HPM_LWIP_H
#include "hpm_enet_drv.h"
#include "hpm_rtl8211.h"
#include "hpm_rtl8211_regs.h"
#include "hpm_rtl8201.h"
#include "hpm_rtl8201_regs.h"
#include "board.h"

#include "lwip/err.h"
#include "lwip/netif.h"

#define ENET_TX_BUFF_COUNT  (16)
#define ENET_RX_BUFF_COUNT  (96)
#define ENET_RX_BUFF_SIZE   ENET_MAX_FRAME_SIZE
#define ENET_TX_BUFF_SIZE   ENET_MAX_FRAME_SIZE

struct HpmEnetDevice {
    int isEnable;
    int isDefault;
    const char *name;
    struct netif netif;
    uint8_t macAddr[6];
    uint8_t ip[4];
    uint8_t gw[4];
    uint8_t netmask[4];
    ENET_Type * base;
    uint32_t irqNum;
    enet_inf_type_t infType;
    enet_desc_t desc;
    enet_mac_config_t mac;
    uint32_t rxSemHandle;
    enet_phy_status_t last_status;
};

#endif

