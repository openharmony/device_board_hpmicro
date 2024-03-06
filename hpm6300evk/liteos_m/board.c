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

#include "board.h"
#include <hpm_pllctlv2_drv.h>
#include <hpm_enet_drv.h>
#include <hpm_gpio_drv.h>
#include <hpm_pmp_drv.h>
#include <hpm_pcfg_drv.h>
/**
 * @brief FLASH configuration option definitions:
 * option[0]:
 *    [31:16] 0xfcf9 - FLASH configuration option tag
 *    [15:4]  0 - Reserved
 *    [3:0]   option words (exclude option[0])
 * option[1]:
 *    [31:28] Flash probe type
 *      0 - SFDP SDR / 1 - SFDP DDR
 *      2 - 1-4-4 Read (0xEB, 24-bit address) / 3 - 1-2-2 Read(0xBB, 24-bit address)
 *      4 - HyperFLASH 1.8V / 5 - HyperFLASH 3V
 *      6 - OctaBus DDR (SPI -> OPI DDR)
 *      8 - Xccela DDR (SPI -> OPI DDR)
 *      10 - EcoXiP DDR (SPI -> OPI DDR)
 *    [27:24] Command Pads after Power-on Reset
 *      0 - SPI / 1 - DPI / 2 - QPI / 3 - OPI
 *    [23:20] Command Pads after Configuring FLASH
 *      0 - SPI / 1 - DPI / 2 - QPI / 3 - OPI
 *    [19:16] Quad Enable Sequence (for the device support SFDP 1.0 only)
 *      0 - Not needed
 *      1 - QE bit is at bit 6 in Status Register 1
 *      2 - QE bit is at bit1 in Status Register 2
 *      3 - QE bit is at bit7 in Status Register 2
 *      4 - QE bit is at bit1 in Status Register 2 and should be programmed by 0x31
 *    [15:8] Dummy cycles
 *      0 - Auto-probed / detected / default value
 *      Others - User specified value, for DDR read, the dummy cycles should be 2 * cycles on FLASH datasheet
 *    [7:4] Misc.
 *      0 - Not used
 *      1 - SPI mode
 *      2 - Internal loopback
 *      3 - External DQS
 *    [3:0] Frequency option
 *      1 - 30MHz / 2 - 50MHz / 3 - 66MHz / 4 - 80MHz / 5 - 100MHz / 6 - 120MHz / 7 - 133MHz / 8 - 166MHz
 *
 * option[2] (Effective only if the bit[3:0] in option[0] > 1)
 *    [31:20]  Reserved
 *    [19:16] IO voltage
 *      0 - 3V / 1 - 1.8V
 *    [15:12] Pin group
 *      0 - 1st group / 1 - 2nd group
 *    [11:8] Connection selection
 *      0 - CA_CS0 / 1 - CB_CS0 / 2 - CA_CS0 + CB_CS0 (Two FLASH connected to CA and CB respectively)
 *    [7:0] Drive Strength
 *      0 - Default value
 * option[3] (Effective only if the bit[3:0] in option[0] > 2, required only for the QSPI NOR FLASH that not supports
 *              JESD216)
 *    [31:16] reserved
 *    [15:12] Sector Erase Command Option, not required here
 *    [11:8]  Sector Size Option, not required here
 *    [7:0] Flash Size Option
 *      0 - 4MB / 1 - 8MB / 2 - 16MB
 */
#if defined(FLASH_XIP) && FLASH_XIP
__attribute__ ((section(".nor_cfg_option"))) const uint32_t option[4] = {0xfcf90001, 0x00000007, 0x0, 0x0};
#endif

void board_delay_us(uint32_t us)
{
    clock_cpu_delay_us(us);
}

void board_delay_ms(uint32_t ms)
{
    clock_cpu_delay_ms(ms);
}

void board_init_clock(void)
{
    uint32_t cpu0_freq = clock_get_frequency(clock_cpu0);
    if (cpu0_freq == PLLCTL_SOC_PLL_REFCLK_FREQ) {
        /* Configure the External OSC ramp-up time: ~9ms */
        pllctlv2_xtal_set_rampup_time(HPM_PLLCTLV2, 32UL * 1000UL * 9U);

        /* Select clock setting preset1 */
        sysctl_clock_set_preset(HPM_SYSCTL, 2);
    }

    /* Add most Clocks to group 0 */
    /* not open uart clock in this API, uart should configure pin function before opening clock */
    clock_add_to_group(clock_cpu0, 0);
    clock_add_to_group(clock_ahbp, 0);
    clock_add_to_group(clock_axic, 0);
    clock_add_to_group(clock_axis, 0);

    clock_add_to_group(clock_mchtmr0, 0);
    clock_add_to_group(clock_femc, 0);
    clock_add_to_group(clock_xpi0, 0);
    clock_add_to_group(clock_xpi1, 0);
    clock_add_to_group(clock_gptmr0, 0);
    clock_add_to_group(clock_gptmr1, 0);
    clock_add_to_group(clock_gptmr2, 0);
    clock_add_to_group(clock_gptmr3, 0);
    clock_add_to_group(clock_uart0, 0);
    clock_add_to_group(clock_i2c0, 0);
    clock_add_to_group(clock_i2c1, 0);
    clock_add_to_group(clock_i2c2, 0);
    clock_add_to_group(clock_i2c3, 0);
    clock_add_to_group(clock_spi0, 0);
    clock_add_to_group(clock_spi1, 0);
    clock_add_to_group(clock_spi2, 0);
    clock_add_to_group(clock_spi3, 0);
    clock_add_to_group(clock_can0, 0);
    clock_add_to_group(clock_can1, 0);
    clock_add_to_group(clock_sdxc0, 0);
    clock_add_to_group(clock_ptpc, 0);
    clock_add_to_group(clock_ref0, 0);
    clock_add_to_group(clock_ref1, 0);
    clock_add_to_group(clock_watchdog0, 0);
    clock_add_to_group(clock_eth0, 0);
    clock_add_to_group(clock_sdp, 0);
    clock_add_to_group(clock_xdma, 0);
    clock_add_to_group(clock_ram0, 0);
    clock_add_to_group(clock_usb0, 0);
    clock_add_to_group(clock_kman, 0);
    clock_add_to_group(clock_gpio, 0);
    clock_add_to_group(clock_mbx0, 0);
    clock_add_to_group(clock_hdma, 0);
    clock_add_to_group(clock_rng, 0);
    clock_add_to_group(clock_mot0, 0);
    clock_add_to_group(clock_mot1, 0);
    clock_add_to_group(clock_acmp, 0);
    clock_add_to_group(clock_dao, 0);
    clock_add_to_group(clock_msyn, 0);
    clock_add_to_group(clock_lmm0, 0);
    clock_add_to_group(clock_pdm, 0);

    clock_add_to_group(clock_adc0, 0);
    clock_add_to_group(clock_adc1, 0);
    clock_add_to_group(clock_adc2, 0);

    clock_add_to_group(clock_dac0, 0);

    clock_add_to_group(clock_i2s0, 0);
    clock_add_to_group(clock_i2s1, 0);

    clock_add_to_group(clock_ffa0, 0);
    clock_add_to_group(clock_tsns, 0);

    /* Connect Group0 to CPU0 */
    clock_connect_group_to_cpu(0, 0);

    /* Configure CPU to 480MHz, AXI/AHB to 160MHz */
    sysctl_config_cpu0_domain_clock(HPM_SYSCTL, clock_source_pll1_clk0, 1, 3, 3);
    /* Configure PLL1_CLK0 Post Divider to 1.2 */
    pllctlv2_set_postdiv(HPM_PLLCTLV2, 1, 0, 1);
    /* Configure PLL1 clock frequencey to 576MHz, the PLL1_CLK0 frequency = 576MHz / 1.2 = 480MHz */
    pllctlv2_init_pll_with_freq(HPM_PLLCTLV2, 1, 576000000);
    clock_update_core_clock();

    /* Configure mchtmr to 24MHz */
    clock_set_source_divider(clock_mchtmr0, clk_src_osc24m, 1);
}

void board_init_pmp(void)
{
    extern uint32_t __noncacheable_start__[];
    extern uint32_t __noncacheable_end__[];

    uint32_t start_addr = (uint32_t) __noncacheable_start__;
    uint32_t end_addr = (uint32_t) __noncacheable_end__;
    uint32_t length = end_addr - start_addr;

    if (length == 0) {
        return;
    }

    /* Ensure the address and the length are power of 2 aligned */
    assert((length & (length - 1U)) == 0U);
    assert((start_addr & (length - 1U)) == 0U);

    pmp_entry_t pmp_entry[3] = {0};
    pmp_entry[0].pmp_addr = PMP_NAPOT_ADDR(0x0000000, 0x80000000);
    pmp_entry[0].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);


    pmp_entry[1].pmp_addr = PMP_NAPOT_ADDR(0x80000000, 0x80000000);
    pmp_entry[1].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);

    pmp_entry[2].pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
    pmp_entry[2].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
    pmp_entry[2].pma_addr = PMA_NAPOT_ADDR(start_addr, length);
    pmp_entry[2].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);
    pmp_config(&pmp_entry[0], ARRAY_SIZE(pmp_entry));
}

void board_init(void)
{
    pcfg_dcdc_set_voltage(HPM_PCFG, 1100);
    board_init_clock();
    board_init_pmp();
}

void board_print_clock_freq(void)
{
    printf("==============================\n");
    printf(" %s clock summary\n", "HPM6300EVK");
    printf("==============================\n");
    printf("cpu0:\t\t %luHz\n", clock_get_frequency(clock_cpu0));
    printf("axi:\t\t %luHz\n", clock_get_frequency(clock_axi));
    printf("ahb:\t\t %luHz\n", clock_get_frequency(clock_ahb));
    printf("mchtmr0:\t %luHz\n", clock_get_frequency(clock_mchtmr0));
    printf("xpi0:\t\t %luHz\n", clock_get_frequency(clock_xpi0));
    printf("xpi1:\t\t %luHz\n", clock_get_frequency(clock_xpi1));
    printf("femc:\t\t %luHz\n", clock_get_frequency(clock_femc));
    printf("==============================\n");
}

void board_print_banner(void)
{
    const uint8_t banner[] = {"\n\
----------------------------------------------------------------------\r\n\
$$\\   $$\\ $$$$$$$\\  $$\\      $$\\ $$\\\r\n\
$$ |  $$ |$$  __$$\\ $$$\\    $$$ |\\__|\r\n\
$$ |  $$ |$$ |  $$ |$$$$\\  $$$$ |$$\\  $$$$$$$\\  $$$$$$\\   $$$$$$\\\r\n\
$$$$$$$$ |$$$$$$$  |$$\\$$\\$$ $$ |$$ |$$  _____|$$  __$$\\ $$  __$$\\\r\n\
$$  __$$ |$$  ____/ $$ \\$$$  $$ |$$ |$$ /      $$ |  \\__|$$ /  $$ |\r\n\
$$ |  $$ |$$ |      $$ |\\$  /$$ |$$ |$$ |      $$ |      $$ |  $$ |\r\n\
$$ |  $$ |$$ |      $$ | \\_/ $$ |$$ |\\$$$$$$$\\ $$ |      \\$$$$$$  |\r\n\
\\__|  \\__|\\__|      \\__|     \\__|\\__| \\_______|\\__|       \\______/\r\n\
----------------------------------------------------------------------\r\n"};
    printf("%s", banner);
}

static void init_enet_pins(ENET_Type *ptr)
{
    if (ptr == HPM_ENET0) {
        HPM_IOC->PAD[IOC_PAD_PA16].FUNC_CTL = IOC_PA16_FUNC_CTL_ETH0_MDC;
        HPM_IOC->PAD[IOC_PAD_PA15].FUNC_CTL = IOC_PA15_FUNC_CTL_ETH0_MDIO;

        HPM_IOC->PAD[IOC_PAD_PA18].FUNC_CTL = IOC_PA18_FUNC_CTL_ETH0_RXD_0;
        HPM_IOC->PAD[IOC_PAD_PA17].FUNC_CTL = IOC_PA17_FUNC_CTL_ETH0_RXD_1;
        HPM_IOC->PAD[IOC_PAD_PA19].FUNC_CTL = IOC_PA19_FUNC_CTL_ETH0_RXDV;

        HPM_IOC->PAD[IOC_PAD_PA20].FUNC_CTL = IOC_PA20_FUNC_CTL_ETH0_TXD_0;
        HPM_IOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_ETH0_TXD_1;
        HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PA23_FUNC_CTL_ETH0_TXEN;

        HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PAD_FUNC_CTL_LOOP_BACK_MASK | IOC_PA22_FUNC_CTL_ETH0_REFCLK;
    }
}

hpm_stat_t board_init_enet_pins(ENET_Type *ptr)
{
    init_enet_pins(ptr);

    return status_success;
}

hpm_stat_t board_init_enet_ptp_clock(ENET_Type *ptr)
{
    /* set clock source */
    if (ptr == HPM_ENET0) {
        /* make sure pll0_clk0 output clock at 400MHz to get a clock at 100MHz for ent0 ptp clock */
        clock_set_source_divider(clock_ptp0, clk_src_pll0_clk0, 4); /* 100MHz */
    } else {
        return status_invalid_argument;
    }

    return status_success;
}

hpm_stat_t board_init_enet_rmii_reference_clock(ENET_Type *ptr, bool internal)
{
    /* Configure Enet clock to output reference clock */
    if (ptr == HPM_ENET0) {
        if (internal) {
            /* set pll output frequency at 1GHz */
            if (pllctlv2_init_pll_with_freq(HPM_PLLCTLV2, PLLCTLV2_PLL_PLL2, 1000000000UL) == status_success) {
                /* set pll2_clk1 output frequence at 250MHz from PLL2 divided by 4 (1 + 15 / 5) */
                pllctlv2_set_postdiv(HPM_PLLCTLV2, PLLCTLV2_PLL_PLL2, 1, 15);
                /* set eth clock frequency at 50MHz for enet0 */
                clock_set_source_divider(clock_eth0, clk_src_pll2_clk1, 5);
            } else {
                return status_fail;
            }
        }
    } else {
        return status_invalid_argument;
    }

    enet_rmii_enable_clock(ptr, internal);

    return status_success;
}

void init_gpio_pins(void)
{
    HPM_BIOC->PAD[IOC_PAD_PZ02].FUNC_CTL = IOC_PZ02_FUNC_CTL_SOC_PZ_02;
    HPM_BIOC->PAD[IOC_PAD_PZ02].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);
    HPM_BIOC->PAD[IOC_PAD_PZ03].FUNC_CTL = IOC_PZ03_FUNC_CTL_SOC_PZ_03;
    HPM_BIOC->PAD[IOC_PAD_PZ03].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);
    
    HPM_IOC->PAD[IOC_PAD_PZ02].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PZ02].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PZ03].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PZ03].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1) | IOC_PAD_PAD_CTL_HYS_SET(1);
}

void init_gpio_out_task_pins(void)
{
    HPM_IOC->PAD[IOC_PAD_PA07].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PA07].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PA08].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PA08].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);

    HPM_IOC->PAD[IOC_PAD_PA09].FUNC_CTL = 0;
    HPM_IOC->PAD[IOC_PAD_PA09].PAD_CTL = IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
}

void init_i2c_pins(I2C_Type *ptr)
{
    if (ptr == HPM_I2C0) {
        HPM_IOC->PAD[IOC_PAD_PC13].FUNC_CTL = IOC_PC13_FUNC_CTL_I2C0_SCL
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC14].FUNC_CTL = IOC_PC14_FUNC_CTL_I2C0_SDA
                                            | IOC_PAD_FUNC_CTL_LOOP_BACK_MASK;
        HPM_IOC->PAD[IOC_PAD_PC13].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
        HPM_IOC->PAD[IOC_PAD_PC14].PAD_CTL = IOC_PAD_PAD_CTL_OD_SET(1) | IOC_PAD_PAD_CTL_PE_SET(1) | IOC_PAD_PAD_CTL_PS_SET(1);
    } else {
        while (1) {
        }
    }
}

void init_spi_pins(SPI_Type *ptr)
{
    if (ptr == HPM_SPI3) {
        HPM_IOC->PAD[IOC_PAD_PC18].FUNC_CTL = IOC_PC18_FUNC_CTL_SPI3_CSN;
        HPM_IOC->PAD[IOC_PAD_PC21].FUNC_CTL = IOC_PC21_FUNC_CTL_SPI3_MOSI;
        HPM_IOC->PAD[IOC_PAD_PC19].FUNC_CTL = IOC_PC19_FUNC_CTL_SPI3_MISO;
        HPM_IOC->PAD[IOC_PAD_PC20].FUNC_CTL = IOC_PC20_FUNC_CTL_SPI3_SCLK | IOC_PAD_FUNC_CTL_LOOP_BACK_SET(1);
    }
}
