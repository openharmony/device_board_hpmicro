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

#define BOARD_CPU_FREQ (480000000UL)

/*fix build error base on board*/
#define IOC_PZ02_FUNC_CTL_SOC_PZ_02 IOC_PZ02_FUNC_CTL_GPIO_Z_02
#define IOC_PZ03_FUNC_CTL_SOC_PZ_03 IOC_PZ03_FUNC_CTL_GPIO_Z_03
#define IOC_PAD_PAD_CTL_SMT_SET IOC_PAD_PAD_CTL_HYS_SET

void board_init(void);
void board_print_clock_freq(void);
void board_print_banner(void);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
