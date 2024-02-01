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

#include <hpm_clock_drv.h>
#include <stdio.h>
#include <los_interrupt.h>
#include "hpm_littlefs_drv.h"
#include "hpm_csr_regs.h"
#include "hpm_l1c_drv.h"

extern struct HpmLittlefsCfg g_hpmLittlefsCfgs[];

xpi_nor_config_t *qiang_debug = NULL;

int HpmLittlefsRead(int partition, UINT32 *offset, void *buf, UINT32 size)
{
    struct HpmLittleCtx *ctx = &g_hpmLittlefsCfgs[partition].ctx;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = *offset;

    uint32_t intSave = LOS_IntLock();
    qiang_debug = &(ctx->xpiNorConfig);
    hpm_stat_t status = rom_xpi_nor_read(base, xpi_xfer_channel_auto,
                                 &ctx->xpiNorConfig, (uint32_t *)buf, chipOffset, size);
    __asm volatile("fence.i");
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("[%s]: read addr: %u, size: %u status: %d failed!!!\n", ctx->mountPoint, chipOffset, size,status);
        return -1;
    }
    return 0;
}


int HpmLittlefsProg(int partition, UINT32 *offset, const void *buf, UINT32 size)
{
    struct HpmLittleCtx *ctx = &g_hpmLittlefsCfgs[partition].ctx;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = *offset;

    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_program(base, xpi_xfer_channel_auto,
                                 &ctx->xpiNorConfig, (const uint32_t *)buf, chipOffset, size);

    __asm volatile("fence.i"); /* mandatory, very important!!! */
    LOS_IntRestore(intSave);
    
    if (status != status_success) {
        printf("[%s]: program addr: %u, size: %u failed!!!\n", ctx->mountPoint, chipOffset, size);
        return -1;
    }
    return 0;
}

int HpmLittlefsErase(int partition, UINT32 offset, UINT32 size)
{
    struct HpmLittleCtx *ctx = &g_hpmLittlefsCfgs[partition].ctx;
    XPI_Type *base = (XPI_Type *)ctx->base;
    uint32_t chipOffset = offset;
    
    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_erase_sector(base, xpi_xfer_channel_auto, &ctx->xpiNorConfig, chipOffset);
    __asm volatile("fence.i"); /* mandatory, very important!!! */
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("[%s]: erase addr: %u, size: %u failed!!!\n", ctx->mountPoint, chipOffset, size);
        return -1;
    }

    return 0;
}


#define HPMICRO_FLASH_SELFTEST_ENABLE 0

#if HPMICRO_FLASH_SELFTEST_ENABLE == 1

static uint8_t rbuf[4096];
static uint8_t wbuf[4096];

static void SelfTest(struct HpmLittlefsCfg *lfsPart)
{
    struct HpmLittleCtx *ctx = &lfsPart->ctx;
    struct PartitionCfg *cfg = &lfsPart->cfg;

    printf("[%s]: Self Test Start\n", ctx->mountPoint);

    uint32_t block = 0;
    uint32_t start = 0;
    uint32_t testSize = (cfg->blockSize > sizeof(wbuf)) ? sizeof(wbuf) : cfg->blockSize;
    printf("[%s]: Self Test testSize: %u\n", ctx->mountPoint, testSize);

    for (int i = 0; i < testSize; i++) {
        wbuf[i] = i & 0xff;
    }

    uint32_t rsetp = 1024;
    uint32_t wsetp = 1024;

    for (block = 0; block < cfg->blockCount; block++) {
        HpmLittlefsErase(cfg->partNo, ctx->startOffset + block * cfg->blockSize, cfg->blockSize);
        printf("Erase OK\n");
        uint32_t offset;
        for (start = 0; start < testSize; start += wsetp) {
            offset = ctx->startOffset + block * cfg->blockSize + start;
            HpmLittlefsProg(cfg->partNo, (UINT32 *)&offset, (const void *)(wbuf + start), wsetp);
        }
        printf("Program OK\n");
        
        for (start = 0; start < testSize; start += rsetp) {
            offset = ctx->startOffset + block * cfg->blockSize + start;
            HpmLittlefsRead(cfg->partNo, (UINT32 *)&offset, (void *)(rbuf + start), rsetp);
        }
        printf("Read OK\n");

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

int HpmLittlefsDriverInit(struct HpmLittlefsCfg *lfsPart)
{
    struct HpmLittleCtx *ctx = &lfsPart->ctx;
    struct PartitionCfg *cfg = &lfsPart->cfg;
    XPI_Type *base = (XPI_Type *)ctx->base;

    if (ctx->isInited) {
        return 0;
    }
    ctx->isInited = 1;

    printf("hpm lfs: mountPoint: %s\n", ctx->mountPoint);
    printf("hpm lfs: startOffset: %u\n", ctx->startOffset);
    printf("hpm lfs: len: %u\n", ctx->len);
    
    xpi_nor_config_option_t option;
    option.header.U = 0xfcf90001U;
    option.option0.U = 0x00000005U;
    option.option1.U = 0x00001000U;
    uint32_t blockSize;
    uint32_t blockCount;
    uint32_t intSave = LOS_IntLock();
    hpm_stat_t status = rom_xpi_nor_auto_config(base, &ctx->xpiNorConfig, &option);
    rom_xpi_nor_get_property(base, &ctx->xpiNorConfig, xpi_nor_property_sector_size, &blockSize);
    __asm volatile("fence.i");
    LOS_IntRestore(intSave);
    if (status != status_success) {
        printf("Error: rom_xpi_nor_auto_config\n");
        while (1);
    }

    blockCount = ctx->len / blockSize;
    printf("hpm lfs: blockCount: %u\n", blockCount);
    printf("hpm lfs: blockSize: %u\n", blockSize);
    printf("------------------------------------------\n");
    
    cfg->blockSize = blockSize;
    cfg->blockCount = blockCount;
#if HPMICRO_FLASH_SELFTEST_ENABLE == 1
    SelfTest(lfsPart);
#endif

    return 0;
}


