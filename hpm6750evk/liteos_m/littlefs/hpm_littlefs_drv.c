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

int HpmLittlefsRead(const struct lfs_config *cfg, lfs_block_t block,
                        lfs_off_t off, void *buffer, lfs_size_t size)
{
    struct HpmLittleCtx *ctx = (struct HpmLittleCtx *)cfg->context;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = cfg->block_size * block + ctx->startOffset + off;

    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_read(base, xpi_xfer_channel_auto,
                                 &ctx->xpiNorConfig, (uint32_t *)buffer, chipOffset, size);
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("[%s]: read addr: %u, size: %u failed!!!\n", ctx->mountPoint, chipOffset, size);
        return -1;
    }
    return 0;
}

int HpmLittlefsProg(const struct lfs_config *cfg, lfs_block_t block,
                        lfs_off_t off, const void *buffer, lfs_size_t size)
{
    struct HpmLittleCtx *ctx = (struct HpmLittleCtx *)cfg->context;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = cfg->block_size * block + ctx->startOffset + off;

    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_program(base, xpi_xfer_channel_auto,
                                 &ctx->xpiNorConfig, (const uint32_t *)buffer, chipOffset, size);
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("[%s]: program addr: %u, size: %u failed!!!\n", ctx->mountPoint, chipOffset, size);
        return -1;
    }
    return 0;
}

int HpmLittlefsErase(const struct lfs_config *cfg, lfs_block_t block)
{
    struct HpmLittleCtx *ctx = (struct HpmLittleCtx *)cfg->context;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = cfg->block_size * block + ctx->startOffset;
    
    uint32_t intSave = LOS_IntLock();

    hpm_stat_t status = rom_xpi_nor_erase(base, xpi_xfer_channel_auto, &ctx->xpiNorConfig,
                                           chipOffset, cfg->block_size);                    
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("[%s]: erase addr: %u, size: %u failed!!!\n", ctx->mountPoint, chipOffset, cfg->block_size);
        return -1;
    }

    return 0;
}

int HpmLittlefsSync(const struct lfs_config *cfg)
{
    return LFS_ERR_OK;
}

#define HPMICRO_FLASH_SELFTEST_ENABLE 0

#if HPMICRO_FLASH_SELFTEST_ENABLE == 1

static uint8_t rbuf[2048];
static uint8_t wbuf[2048];

static void SelfTest(const struct lfs_config *cfg)
{
    struct HpmLittleCtx *ctx = (struct HpmLittleCtx *)cfg->context;
    printf("[%s]: Self Test Start\n", ctx->mountPoint);

    lfs_block_t block = 0;
    uint32_t start = 0;
    uint32_t testSize = (cfg->block_size > sizeof(wbuf)) ? sizeof(wbuf) : cfg->block_size;
    printf("[%s]: Self Test testSize: %u\n", ctx->mountPoint, testSize);

    for (int i = 0; i < testSize; i++) {
        wbuf[i] = i & 0xff;
    }

    uint32_t rsetp = cfg->read_size;
    uint32_t wsetp = cfg->prog_size;

    for (block = 0; block < cfg->block_count; block++) {
        HpmLittlefsErase(cfg, block);
        for (start = 0; start < testSize; start += wsetp) {
            HpmLittlefsProg(cfg, block, start, wbuf + start, wsetp);
        }
        
        for (start = 0; start < testSize; start += rsetp) {
            HpmLittlefsRead(cfg, block, start, rbuf + start, rsetp);
        }

        for (int i = 0; i < testSize; i++) {
            if (wbuf[i] != rbuf[i]) {
                printf("%d: wbuf(%u) != rbuf(%u)\n", i, wbuf[i], rbuf[i]);
            }  
        }

        int isEqu = memcmp(wbuf, rbuf, testSize);
        if (isEqu) {
            printf("[%s]: block: %u fail\n", ctx->mountPoint, block);
            break;
        } else {
            printf("[%s]: block: %u pass\n", ctx->mountPoint, block);
        }
    }
}
#endif

int HpmLittlefsDriverInit(const struct lfs_config *cfg)
{
    struct HpmLittleCtx *ctx = (struct HpmLittleCtx *)cfg->context;
    XPI_Type *base = (XPI_Type *)ctx->base;

    if (ctx->isInited) {
        return 0;
    }
    ctx->isInited = 1;

    printf("hpm lfs: startOffset: %u\n", ctx->startOffset);
    printf("hpm lfs: mountPoint: %s\n", ctx->mountPoint);
    printf("hpm lfs: block_count: %u\n", cfg->block_count);
    printf("hpm lfs: block_size: %u\n", cfg->block_size);

    xpi_nor_config_option_t option;
    option.header.U = 0xfcf90001U;
    option.option0.U = 0x00000005U;
    option.option1.U = 0x00001000U;

    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_auto_config(base, &ctx->xpiNorConfig, &option);
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("Error: rom_xpi_nor_auto_config\n");
        while (1);
    }
#if HPMICRO_FLASH_SELFTEST_ENABLE == 1
    SelfTest(cfg);
#endif

    return 0;
}


