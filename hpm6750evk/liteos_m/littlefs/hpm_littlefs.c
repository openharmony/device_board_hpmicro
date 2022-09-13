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

struct HpmLittlefsCfg {
    struct lfs_config cfg;
    struct HpmLittleCtx ctx;
};

static struct HpmLittlefsCfg g_hpmLittlefsCfgs[] = {
    [0] = {
        .cfg = {
            .read  = HpmLittlefsRead,
            .prog  = HpmLittlefsProg,
            .erase = HpmLittlefsErase,
            .sync  = HpmLittlefsSync,

            .read_size = 16,
            .prog_size = 16,
            .cache_size = 1024,
            .lookahead_size = 16,
            .block_cycles = 1000,
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
    for (int i = 0; i < num; i++) {
        g_hpmLittlefsCfgs[i].cfg.context = &g_hpmLittlefsCfgs[i].ctx;
        HpmLittlefsDriverInit(&g_hpmLittlefsCfgs[i].cfg);

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

