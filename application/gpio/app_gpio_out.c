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

static void GpioDriverOutTestTask(uint32_t arg)
{
    HILOG_INFO(HILOG_MODULE_APP, "GpioDriverOutTestTask");

    HPM_IOC->PAD[IOC_PAD_PB11].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PB11].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PB12].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PB12].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PB13].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PB13].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    GpioSetDir(GPIOB(11), GPIO_DIR_OUT);
    GpioSetDir(GPIOB(12), GPIO_DIR_OUT);
    GpioSetDir(GPIOB(13), GPIO_DIR_OUT);

    while (1) {
        GpioWrite(GPIOB(11), GPIO_VAL_LOW);
        LOS_TaskDelay(500);
        GpioWrite(GPIOB(11), GPIO_VAL_HIGH);
        LOS_TaskDelay(500);
        GpioWrite(GPIOB(12), GPIO_VAL_LOW);
        LOS_TaskDelay(500);
        GpioWrite(GPIOB(12), GPIO_VAL_HIGH);
        LOS_TaskDelay(500);
        GpioWrite(GPIOB(13), GPIO_VAL_LOW);
        LOS_TaskDelay(500);
        GpioWrite(GPIOB(13), GPIO_VAL_HIGH);
        LOS_TaskDelay(500);
    }
}

static void GpioDriverTest(void)
{
    TSK_INIT_PARAM_S taskInitParam = {0};
    
    taskInitParam.pcName = "gpio_out_test";
    taskInitParam.pfnTaskEntry = GpioDriverOutTestTask;
    taskInitParam.stackAddr = 0;
    taskInitParam.uwStackSize = 4096;
    taskInitParam.usTaskPrio = 20;
    taskInitParam.uwArg = 0x66;
    uint32_t taskID;
    LOS_TaskCreate(&taskID, &taskInitParam);
}

APP_FEATURE_INIT(GpioDriverTest);
