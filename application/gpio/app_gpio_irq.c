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
#include <gpio_if.h>
#include <hpm_soc.h>
#include <hpm_clock_drv.h>

#define LOG_TAG "HPM_GPIO_OUT"

#define GPIOA(pin) (pin)
#define GPIOB(pin) (pin + 32)
#define GPIOC(pin) (pin + 32 * 2)
#define GPIOD(pin) (pin + 32 * 3)
#define GPIOE(pin) (pin + 32 * 4)
#define GPIOF(pin) (pin + 32 * 5)
#define GPIOY(pin) (pin + 32 * 14)
#define GPIOZ(pin) (pin + 32 * 15)


static int32_t GpioIrq(uint16_t gpio, void *data)
{
    HILOG_INFO(HILOG_MODULE_APP, "gpio: %u", gpio);
}

static void GpioDriverInTestTask(void)
{
    HILOG_INFO(HILOG_MODULE_APP, "GpioDriverIrqTest");

    HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_SOC_PZ_02;
    HPM_BIOC->PAD[IOC_PAD_PZ02].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_SMT_SET(1);
    HPM_BIOC->PAD[IOC_PAD_PZ03].FUNC_CTL = IOC_PZ03_FUNC_CTL_SOC_PZ_03;
    HPM_BIOC->PAD[IOC_PAD_PZ03].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_SMT_SET(1);
    
    HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PZ02].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_SMT_SET(1);

    HPM_IOC->PAD[IOC_PAD_PZ03].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PZ03].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_SMT_SET(1);

    GpioSetDir(GPIOZ(2), GPIO_DIR_IN);
    GpioSetDir(GPIOZ(3), GPIO_DIR_IN);

    GpioSetIrq(GPIOZ(2), GPIO_IRQ_TRIGGER_RISING, GpioIrq, NULL);
    GpioSetIrq(GPIOZ(3), GPIO_IRQ_TRIGGER_FALLING, GpioIrq, NULL);

    GpioEnableIrq(GPIOZ(2));
    GpioEnableIrq(GPIOZ(3));
}

APP_FEATURE_INIT(GpioDriverInTestTask);
