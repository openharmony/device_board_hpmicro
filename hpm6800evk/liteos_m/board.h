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

#ifndef _HPM_BOARD_H
#define _HPM_BOARD_H
#include <stdio.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"

#define BOARD_NAME "hpm6800evk"
#define BOARD_CPU_FREQ (500000000UL)


/* enet section */
#define BOARD_ENET_COUNT                (1U)
#define BOARD_ENET_PPS                  HPM_ENET0
#define BOARD_ENET_PPS_IDX              enet_pps_0
#define BOARD_ENET_PPS_PTP_CLOCK        clock_ptp0

#define BOARD_ENET_RGMII_PHY_ITF        enet_inf_rgmii
#define BOARD_ENET_RGMII_RST_GPIO       HPM_GPIO0
#define BOARD_ENET_RGMII_RST_GPIO_INDEX GPIO_DO_GPIOD
#define BOARD_ENET_RGMII_RST_GPIO_PIN   (18U)
#define BOARD_ENET_RGMII                HPM_ENET0
#define BOARD_ENET_RGMII_TX_DLY         (0U)
#define BOARD_ENET_RGMII_RX_DLY         (0U)
#define BOARD_ENET_RGMII_PTP_CLOCK      clock_ptp0
#define BOARD_ENET_RGMII_PPS0_PINOUT    (1)

#define TEST_APP_I2C_CLK_NAME clock_i2c1

void board_init(void);
void board_print_clock_freq(void);
void board_print_banner(void);

hpm_stat_t board_reset_enet_phy(ENET_Type *ptr);
hpm_stat_t board_init_enet_pins(ENET_Type *ptr);
hpm_stat_t board_init_enet_rmii_reference_clock(ENET_Type *ptr, bool internal);
hpm_stat_t board_init_enet_rgmii_clock_delay(ENET_Type *ptr);
hpm_stat_t board_init_enet_ptp_clock(ENET_Type *ptr);
void init_i2c_pins(I2C_Type *ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
