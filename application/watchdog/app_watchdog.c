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
#include <ohos_init.h>
#include <stdio.h>
#include <los_task.h>
#include <stdint.h>
#include <log.h>
#include <watchdog_if.h>

#define LOG_TAG "HPM_WDG"

static void WatchdogDriverApiTask(uint32_t arg)
{
    HILOG_INFO(HILOG_MODULE_APP, "%s", __func__);
    DevHandle handle;
    int32_t status;
    uint32_t seconds;
    int ret;
    ret = WatchdogOpen(2,  &handle);
    if (ret) {
        HILOG_ERROR(HILOG_MODULE_APP, "WatchdogOpen: %d", ret);
        return;
    }

    WatchdogGetStatus(handle, &status);
    if (status != WATCHDOG_STOP) {
        HILOG_INFO(HILOG_MODULE_APP, "WatchdogGetStatus expect <WATCHDOG_STOP>, %u", __LINE__);
    }
    
    WatchdogSetTimeout(handle, 3);
    WatchdogGetTimeout(handle, &seconds);
    if (seconds != 3) {
        HILOG_ERROR(HILOG_MODULE_APP, "WatchdogGetTimeout Failed!!!");
    }

    WatchdogStart(handle);
    WatchdogGetStatus(handle, &status);
    if (status != WATCHDOG_START) {
        HILOG_ERROR(HILOG_MODULE_APP, "WatchdogGetStatus expect <WATCHDOG_START>");
    }

    uint32_t cnt = 3;
    while (cnt--) {
        HILOG_INFO(HILOG_MODULE_APP, "WatchdogFeed");
        WatchdogFeed(handle);
        LOS_TaskDelay(2000);
    }

    WatchdogStop(handle);
    WatchdogGetStatus(handle, &status);
    if (status != WATCHDOG_STOP) {
        HILOG_ERROR(HILOG_MODULE_APP, "WatchdogGetStatus expect <WATCHDOG_STOP>, %u", __LINE__);
    }

    WatchdogClose(handle);
}

static void WatchdogDriverTest(void)
{
    TSK_INIT_PARAM_S taskInitParam = {0};
    
    taskInitParam.pcName = "wdg_test";
    taskInitParam.pfnTaskEntry = WatchdogDriverApiTask;
    taskInitParam.stackAddr = 0;
    taskInitParam.uwStackSize = 4096;
    taskInitParam.usTaskPrio = 20;
    taskInitParam.uwArg = 0x66;
    uint32_t taskID;
    LOS_TaskCreate(&taskID, &taskInitParam);
}

APP_FEATURE_INIT(WatchdogDriverTest);