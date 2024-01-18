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

#include "uart.h"
#include "los_arch_interrupt.h"
#include "los_interrupt.h"
#include "riscv_hal.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

#define RX_BUF_SIZE 128
static uint8_t rx_buf[RX_BUF_SIZE];
static uint16_t rx_index;
static uint16_t tx_index;

INT32 UartPutc(INT32 c, VOID *file)
{
    (VOID) file;
    if (c == '\n') {
        uart_send_byte(HPM_UART0, (UINT8)'\r');
    }
    uart_send_byte(HPM_UART0, (UINT8)c);
    return c;
}

INT32 UartGetc(VOID)
{
    uint8_t c = 0;
    if (tx_index != rx_index) {
        c = rx_buf[tx_index++];
        tx_index %= RX_BUF_SIZE;     
    }
    return c;
}

VOID UartInit(VOID)
{
    HPM_IOC->PAD[IOC_PAD_PA00].FUNC_CTL = IOC_PA00_FUNC_CTL_UART0_TXD;
    HPM_IOC->PAD[IOC_PAD_PA01].FUNC_CTL = IOC_PA01_FUNC_CTL_UART0_RXD;

    uart_config_t config = {0};
    clock_set_source_divider(clock_uart0, clk_src_osc24m, 1U);
    uart_default_config(HPM_UART0, &config);
    config.src_freq_in_hz = clock_get_frequency(clock_uart0);
    config.baudrate = 115200;
    uart_init(HPM_UART0, &config);  
}

VOID UartReceiveHandler(VOID)
{
    if (uart_get_irq_id(HPM_UART0) & uart_intr_id_rx_data_avail) {
        uint8_t c;
        if (status_success == uart_receive_byte(HPM_UART0, &c)) {
            rx_buf[rx_index++] = c;
            rx_index %= RX_BUF_SIZE;
            if (rx_index == tx_index) {
                tx_index++;
                tx_index %= RX_BUF_SIZE;
            }
            (void)LOS_EventWrite(&g_shellInputEvent, 0x1);
        }
    }
    return;
}

VOID Uart0RxIrqRegister(VOID)
{
    uart_enable_irq(HPM_UART0, uart_intr_rx_data_avail_or_timeout);

    uint32_t ret = LOS_HwiCreate(HPM2LITEOS_IRQ(IRQn_UART0), OS_HWI_PRIO_HIGHEST, 0, (HWI_PROC_FUNC)UartReceiveHandler, 0);
    if (ret != LOS_OK) {
        return;
    }
    HalIrqEnable(HPM2LITEOS_IRQ(IRQn_UART0));
}

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
