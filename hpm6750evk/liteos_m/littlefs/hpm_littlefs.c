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

#include <hpm_clock_drv.h>
#include <stdio.h>
#include <los_interrupt.h>
#include "hpm_littlefs_drv.h"
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/mount.h>
#include <sys/stat.h>
#include <dirent.h>
#include <los_fs.h>

struct HpmLittlefsCfg g_hpmLittlefsCfgs[] = {
    [0] = {
        .cfg = {
            /* partition low-level read func */
            .readFunc = HpmLittlefsRead,
            /* partition low-level write func */
            .writeFunc = HpmLittlefsProg,
            /* partition low-level erase func */
            .eraseFunc = HpmLittlefsErase,

            .readSize = 16,
            .writeSize = 16,
            .blockSize = 0, /* auto fill in HpmLittlefsDriverInit() */
            .blockCount = 0, /* auto fill in HpmLittlefsDriverInit() */
            .cacheSize = 1024,

            .partNo = 0,
            .lookaheadSize = 16,
            .blockCycles = 1000,
        },
        .ctx = {
            .startOffset = 5 * 1024 * 1024,
            .len = 2 * 1024 * 1024,
            .base = HPM_XPI0_BASE,
            .mountPoint = "/data",
        }
    },
};

void HpmLittlefsInit(void)
{
    uint32_t num = sizeof(g_hpmLittlefsCfgs) / sizeof(g_hpmLittlefsCfgs[0]);
    int ret;

    int *lengthArray = (int *)malloc(num * sizeof(int) * 2);
    int *addrArray = lengthArray + num;

    for (int i = 0; i < num; i++) {
        lengthArray[i] = g_hpmLittlefsCfgs[i].ctx.len;
        addrArray[i] = g_hpmLittlefsCfgs[i].ctx.startOffset;
    }

    LOS_DiskPartition("spiflash", "littlefs", lengthArray, addrArray, num);
    free(lengthArray);

    for (int i = 0; i < num; i++) {
        HpmLittlefsDriverInit(&g_hpmLittlefsCfgs[i]);
        ret = mount(NULL, g_hpmLittlefsCfgs[i].ctx.mountPoint, "littlefs", 0, &g_hpmLittlefsCfgs[i].cfg);
        if (ret < 0) {
            printf("Err: hpm littlefs [%s] mount failed!!!\n", g_hpmLittlefsCfgs[i].ctx.mountPoint);
            continue;
        }

        DIR *dir = NULL;
        if ((dir = opendir(g_hpmLittlefsCfgs[i].ctx.mountPoint)) == NULL) {
            ret = mkdir(g_hpmLittlefsCfgs[i].ctx.mountPoint, S_IRUSR | S_IWUSR);
            if (ret) {
                printf("Err: hpm littlefs mkdir [%s] mount failed!!!\n", g_hpmLittlefsCfgs[i].ctx.mountPoint);
                continue;
            }
        } else {
            closedir(dir);
        }
    }
}

