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
#define TEST_APP_I2C_CLK_NAME clock_i2c0

#define GPIOA(pin) (pin)
#define GPIOB(pin) (pin + 32)
#define GPIOC(pin) (pin + 32 * 2)
#define GPIOD(pin) (pin + 32 * 3)
#define GPIOE(pin) (pin + 32 * 4)
#define GPIOF(pin) (pin + 32 * 5)
#define GPIOY(pin) (pin + 32 * 14)
#define GPIOZ(pin) (pin + 32 * 15)

#define BOARD_GPIO_IN_IRQ_TASK_GPIO1 GPIOB(4)
#define BOARD_GPIO_IN_IRQ_TASK_GPIO2 GPIOB(5)

#define BOARD_GPIO_OUT_TASK_GPIO1 GPIOB(11)
#define BOARD_GPIO_OUT_TASK_GPIO2 GPIOB(12)
#define BOARD_GPIO_OUT_TASK_GPIO3 GPIOB(13)

void board_init(void);
void board_print_clock_freq(void);
void board_print_banner(void);
void init_gpio_pins(void);
void init_gpio_out_task_pins(void);
void init_i2c_pins(I2C_Type *ptr);
void init_spi_pins(SPI_Type *ptr);

#if defined(__cplusplus)
}
#endif /* __cplusplus */
#endif /* _HPM_BOARD_H */
