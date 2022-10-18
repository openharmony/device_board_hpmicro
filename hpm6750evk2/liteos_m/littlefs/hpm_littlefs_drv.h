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
#ifndef HPM_LITTLEFS_DRV_H
#define HPM_LITTLEFS_DRV_H

#include <stdint.h>
#include <hpm_soc.h>
#include <hpm_romapi.h>
#include <los_fs.h>

struct HpmLittleCtx {
    xpi_nor_config_t xpiNorConfig;
    int isInited;
    uint32_t startOffset; /* The partion address in chip; unit in byte */
    uint32_t len; /* The partion length, unit in byte */
    uint32_t base; /* XPI register base */
    char *mountPoint;
};

struct HpmLittlefsCfg {
    struct PartitionCfg cfg;
    struct HpmLittleCtx ctx;
};

int HpmLittlefsRead(int partition, UINT32 *offset, void *buf, UINT32 size);
int HpmLittlefsProg(int partition, UINT32 *offset, const void *buf, UINT32 size);                    
int HpmLittlefsErase(int partition, UINT32 offset, UINT32 size);
int HpmLittlefsDriverInit(struct HpmLittlefsCfg *cfg);

#endif
