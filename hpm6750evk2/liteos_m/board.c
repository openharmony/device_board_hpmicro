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

#include "board.h"
#include <hpm_pllctl_drv.h>
#include <hpm_enet_drv.h>
#include <hpm_gpio_drv.h>
#include <hpm_pmp_drv.h>
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
        pllctl_xtal_set_rampup_time(HPM_PLLCTL, 32UL * 1000UL * 9U);

        /* Select clock setting preset1 */
        sysctl_clock_set_preset(HPM_SYSCTL, sysctl_preset_1);
    }

    /* Add most Clocks to group 0 */
    clock_add_to_group(clock_cpu0, 0);
    clock_add_to_group(clock_mchtmr0, 0);
    clock_add_to_group(clock_axi0, 0);
    clock_add_to_group(clock_axi1, 0);
    clock_add_to_group(clock_axi2, 0);
    clock_add_to_group(clock_ahb, 0);
    clock_add_to_group(clock_dram, 0);
    clock_add_to_group(clock_xpi0, 0);
    clock_add_to_group(clock_xpi1, 0);
    clock_add_to_group(clock_gptmr0, 0);
    clock_add_to_group(clock_gptmr1, 0);
    clock_add_to_group(clock_gptmr2, 0);
    clock_add_to_group(clock_gptmr3, 0);
    clock_add_to_group(clock_gptmr4, 0);
    clock_add_to_group(clock_gptmr5, 0);
    clock_add_to_group(clock_gptmr6, 0);
    clock_add_to_group(clock_gptmr7, 0);
    clock_add_to_group(clock_uart0, 0);
    clock_add_to_group(clock_uart1, 0);
    clock_add_to_group(clock_uart2, 0);
    clock_add_to_group(clock_uart3, 0);
    clock_add_to_group(clock_uart13, 0);
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
    clock_add_to_group(clock_can2, 0);
    clock_add_to_group(clock_can3, 0);
    clock_add_to_group(clock_display, 0);
    clock_add_to_group(clock_sdxc0, 0);
    clock_add_to_group(clock_sdxc1, 0);
    clock_add_to_group(clock_camera0, 0);
    clock_add_to_group(clock_camera1, 0);
    clock_add_to_group(clock_ptpc, 0);
    clock_add_to_group(clock_ref0, 0);
    clock_add_to_group(clock_ref1, 0);
    clock_add_to_group(clock_watchdog0, 0);
    clock_add_to_group(clock_watchdog1, 0);
    clock_add_to_group(clock_watchdog2, 0);
    clock_add_to_group(clock_watchdog3, 0);
    clock_add_to_group(clock_pwdg, 0);
    clock_add_to_group(clock_eth0, 0);
    clock_add_to_group(clock_eth1, 0);
    clock_add_to_group(clock_sdp, 0);
    clock_add_to_group(clock_xdma, 0);
    clock_add_to_group(clock_ram0, 0);
    clock_add_to_group(clock_ram1, 0);
    clock_add_to_group(clock_usb0, 0);
    clock_add_to_group(clock_usb1, 0);
    clock_add_to_group(clock_jpeg, 0);
    clock_add_to_group(clock_pdma, 0);
    clock_add_to_group(clock_kman, 0);
    clock_add_to_group(clock_gpio, 0);
    clock_add_to_group(clock_mbx0, 0);
    clock_add_to_group(clock_hdma, 0);
    clock_add_to_group(clock_rng, 0);
    clock_add_to_group(clock_mot0, 0);
    clock_add_to_group(clock_mot1, 0);
    clock_add_to_group(clock_mot2, 0);
    clock_add_to_group(clock_mot3, 0);
    clock_add_to_group(clock_acmp, 0);
    clock_add_to_group(clock_dao, 0);
    clock_add_to_group(clock_msyn, 0);
    clock_add_to_group(clock_lmm0, 0);
    clock_add_to_group(clock_lmm1, 0);

    clock_add_to_group(clock_adc0, 0);
    clock_add_to_group(clock_adc1, 0);
    clock_add_to_group(clock_adc2, 0);
    clock_add_to_group(clock_adc3, 0);

    clock_add_to_group(clock_i2s0, 0);
    clock_add_to_group(clock_i2s1, 0);
    clock_add_to_group(clock_i2s2, 0);
    clock_add_to_group(clock_i2s3, 0);

    /* Add the CPU1 clock to Group1 */
    clock_add_to_group(clock_mchtmr1, 1);
    clock_add_to_group(clock_mbx1, 1);

    /* Connect Group0 to CPU0 */
    clock_connect_group_to_cpu(0, 0);

    if (status_success != pllctl_init_int_pll_with_freq(HPM_PLLCTL, 0, BOARD_CPU_FREQ)) {
        printf("Failed to set pll0_clk0 to %ldHz\n", BOARD_CPU_FREQ);
        while(1);
    }

    clock_set_source_divider(clock_cpu0, clk_src_pll0_clk0, 1);
    clock_set_source_divider(clock_cpu1, clk_src_pll0_clk0, 1);
    /* Connect Group1 to CPU1 */
    clock_connect_group_to_cpu(1, 1);

    clock_update_core_clock();
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

    pmp_entry_t pmp_entry[1];
    pmp_entry[0].pmp_addr = PMP_NAPOT_ADDR(start_addr, length);
    pmp_entry[0].pmp_cfg.val = PMP_CFG(READ_EN, WRITE_EN, EXECUTE_EN, ADDR_MATCH_NAPOT, REG_UNLOCK);
    pmp_entry[0].pma_addr = PMA_NAPOT_ADDR(start_addr, length);
    pmp_entry[0].pma_cfg.val = PMA_CFG(ADDR_MATCH_NAPOT, MEM_TYPE_MEM_NON_CACHE_BUF, AMO_EN);

    pmp_config(&pmp_entry[0], ARRAY_SIZE(pmp_entry));
}

void board_init_ahb(void)
{
    clock_set_source_divider(clock_ahb, clk_src_pll1_clk1, 2);/*200m hz*/
}

void board_init(void)
{
    board_init_clock();
    sysctl_set_cpu_lp_mode(HPM_SYSCTL, HPM_CORE0, cpu_lp_mode_ungate_cpu_clock);
    board_init_pmp();
    board_init_ahb();
}

void board_print_clock_freq(void)
{
    printf("==============================\r\n");
    printf(" %s clock summary\r\n", "HPM6750EVK2");
    printf("==============================\r\n");
    printf("cpu0:\t\t %dHz\r\n", clock_get_frequency(clock_cpu0));
    printf("cpu1:\t\t %dHz\r\n", clock_get_frequency(clock_cpu1));
    printf("axi0:\t\t %dHz\r\n", clock_get_frequency(clock_axi0));
    printf("axi1:\t\t %dHz\r\n", clock_get_frequency(clock_axi1));
    printf("axi2:\t\t %dHz\r\n", clock_get_frequency(clock_axi2));
    printf("ahb:\t\t %dHz\r\n", clock_get_frequency(clock_ahb));
    printf("mchtmr0:\t %dHz\r\n", clock_get_frequency(clock_mchtmr0));
    printf("mchtmr1:\t %dHz\r\n", clock_get_frequency(clock_mchtmr1));
    printf("xpi0:\t\t %dHz\r\n", clock_get_frequency(clock_xpi0));
    printf("xpi1:\t\t %dHz\r\n", clock_get_frequency(clock_xpi1));
    printf("dram:\t\t %dHz\r\n", clock_get_frequency(clock_dram));
    printf("display:\t %dHz\r\n", clock_get_frequency(clock_display));
    printf("cam0:\t\t %dHz\r\n", clock_get_frequency(clock_camera0));
    printf("cam1:\t\t %dHz\r\n", clock_get_frequency(clock_camera1));
    printf("jpeg:\t\t %dHz\r\n", clock_get_frequency(clock_jpeg));
    printf("pdma:\t\t %dHz\r\n", clock_get_frequency(clock_pdma));
    printf("==============================\r\n");
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
        HPM_IOC->PAD[IOC_PAD_PF00].FUNC_CTL = IOC_PF00_FUNC_CTL_GPIO_F_00;

        HPM_IOC->PAD[IOC_PAD_PE22].FUNC_CTL = IOC_PE22_FUNC_CTL_ETH0_MDC;
        HPM_IOC->PAD[IOC_PAD_PE23].FUNC_CTL = IOC_PE23_FUNC_CTL_ETH0_MDIO;

        HPM_IOC->PAD[IOC_PAD_PD31].FUNC_CTL = IOC_PD31_FUNC_CTL_ETH0_RXD_0;
        HPM_IOC->PAD[IOC_PAD_PE04].FUNC_CTL = IOC_PE04_FUNC_CTL_ETH0_RXD_1;
        HPM_IOC->PAD[IOC_PAD_PE02].FUNC_CTL = IOC_PE02_FUNC_CTL_ETH0_RXD_2;
        HPM_IOC->PAD[IOC_PAD_PE07].FUNC_CTL = IOC_PE07_FUNC_CTL_ETH0_RXD_3;
        HPM_IOC->PAD[IOC_PAD_PE03].FUNC_CTL = IOC_PE03_FUNC_CTL_ETH0_RXCK;
        HPM_IOC->PAD[IOC_PAD_PD30].FUNC_CTL = IOC_PD30_FUNC_CTL_ETH0_RXDV;

        HPM_IOC->PAD[IOC_PAD_PE06].FUNC_CTL = IOC_PE06_FUNC_CTL_ETH0_TXD_0;
        HPM_IOC->PAD[IOC_PAD_PD29].FUNC_CTL = IOC_PD29_FUNC_CTL_ETH0_TXD_1;
        HPM_IOC->PAD[IOC_PAD_PD28].FUNC_CTL = IOC_PD28_FUNC_CTL_ETH0_TXD_2;
        HPM_IOC->PAD[IOC_PAD_PE05].FUNC_CTL = IOC_PE05_FUNC_CTL_ETH0_TXD_3;
        HPM_IOC->PAD[IOC_PAD_PE01].FUNC_CTL = IOC_PE01_FUNC_CTL_ETH0_TXCK;
        HPM_IOC->PAD[IOC_PAD_PE00].FUNC_CTL = IOC_PE00_FUNC_CTL_ETH0_TXEN;
    } else if (ptr == HPM_ENET1) {
        HPM_IOC->PAD[IOC_PAD_PE26].FUNC_CTL = IOC_PE26_FUNC_CTL_GPIO_E_26;

        HPM_IOC->PAD[IOC_PAD_PD11].FUNC_CTL = IOC_PD11_FUNC_CTL_ETH1_MDC;
        HPM_IOC->PAD[IOC_PAD_PD14].FUNC_CTL = IOC_PD14_FUNC_CTL_ETH1_MDIO;

        HPM_IOC->PAD[IOC_PAD_PE20].FUNC_CTL = IOC_PE20_FUNC_CTL_ETH1_RXD_0;
        HPM_IOC->PAD[IOC_PAD_PE18].FUNC_CTL = IOC_PE18_FUNC_CTL_ETH1_RXD_1;
        HPM_IOC->PAD[IOC_PAD_PE15].FUNC_CTL = IOC_PE15_FUNC_CTL_ETH1_RXDV;

        HPM_IOC->PAD[IOC_PAD_PE19].FUNC_CTL = IOC_PE19_FUNC_CTL_ETH1_TXD_0;
        HPM_IOC->PAD[IOC_PAD_PE17].FUNC_CTL = IOC_PE17_FUNC_CTL_ETH1_TXD_1;
        HPM_IOC->PAD[IOC_PAD_PE14].FUNC_CTL = IOC_PE14_FUNC_CTL_ETH1_TXEN;

        HPM_IOC->PAD[IOC_PAD_PE16].FUNC_CTL = IOC_PAD_FUNC_CTL_LOOP_BACK_MASK | IOC_PE16_FUNC_CTL_ETH1_REFCLK;
    }
}

hpm_stat_t board_init_enet_pins(ENET_Type *ptr)
{
    init_enet_pins(ptr);

    if (ptr == HPM_ENET0) {
        gpio_set_pin_output_with_initial(BOARD_ENET_RGMII_RST_GPIO, BOARD_ENET_RGMII_RST_GPIO_INDEX, BOARD_ENET_RGMII_RST_GPIO_PIN, 0);
    } else if (ptr == HPM_ENET1) {
        gpio_set_pin_output_with_initial(BOARD_ENET_RMII_RST_GPIO, BOARD_ENET_RMII_RST_GPIO_INDEX, BOARD_ENET_RMII_RST_GPIO_PIN, 0);
    } else {
        return status_invalid_argument;
    }

    return status_success;
}

hpm_stat_t board_reset_enet_phy(ENET_Type *ptr)
{
    if (ptr == HPM_ENET0) {
        gpio_write_pin(BOARD_ENET_RGMII_RST_GPIO, BOARD_ENET_RGMII_RST_GPIO_INDEX, BOARD_ENET_RGMII_RST_GPIO_PIN, 0);
        board_delay_ms(1);
        gpio_write_pin(BOARD_ENET_RGMII_RST_GPIO, BOARD_ENET_RGMII_RST_GPIO_INDEX, BOARD_ENET_RGMII_RST_GPIO_PIN, 1);
    } else if (ptr == HPM_ENET1) {
        gpio_write_pin(BOARD_ENET_RMII_RST_GPIO, BOARD_ENET_RMII_RST_GPIO_INDEX, BOARD_ENET_RMII_RST_GPIO_PIN, 0);
        board_delay_ms(1);
        gpio_write_pin(BOARD_ENET_RMII_RST_GPIO, BOARD_ENET_RMII_RST_GPIO_INDEX, BOARD_ENET_RMII_RST_GPIO_PIN, 1);
    } else {
        return status_invalid_argument;
    }

    return status_success;
}


hpm_stat_t board_init_enet_ptp_clock(ENET_Type *ptr)
{
    /* set clock source */
    if (ptr == HPM_ENET0) {
        /* make sure pll0_clk0 output clock at 400MHz to get a clock at 100MHz for the enet0 ptp function */
        clock_set_source_divider(clock_ptp0, clk_src_pll1_clk1, 4); /* 100MHz */
    } else if (ptr == HPM_ENET1) {
        /* make sure pll0_clk0 output clock at 400MHz to get a clock at 100MHz for the enet1 ptp function */
        clock_set_source_divider(clock_ptp1, clk_src_pll1_clk1, 4); /* 100MHz */
    } else {
        return status_invalid_argument;
    }

    return status_success;
}

hpm_stat_t board_init_enet_rmii_reference_clock(ENET_Type *ptr, bool internal)
{
    if (internal == false) {
        return status_success;
    }
    /* Configure Enet clock to output reference clock */
    if (ptr == HPM_ENET0) {
        /* make sure pll2_clk1 output clock at 250MHz then set 50MHz for enet0 */
        clock_set_source_divider(clock_eth0, clk_src_pll2_clk1, 5);
    } else if (ptr == HPM_ENET1) {
        /* make sure pll2_clk1 output clock at 250MHz then set 50MHz for enet1 */
        clock_set_source_divider(clock_eth1, clk_src_pll2_clk1, 5); /* set 50MHz for enet1 */
    } else {
        return status_invalid_argument;
    }

    enet_rmii_enable_clock(ptr, internal);

    return status_success;
}

hpm_stat_t board_init_enet_rgmii_clock_delay(ENET_Type *ptr)
{
    return enet_rgmii_set_clock_delay(ptr, BOARD_ENET_RGMII_TX_DLY, BOARD_ENET_RGMII_RX_DLY);
}
