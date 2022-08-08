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

#ifndef _UART_H
#define _UART_H

#include "los_compiler.h"
#include "los_event.h"
#include "los_reg.h"
#include "soc.h"

#include <stdio.h>
#include "hpm_common.h"
#include "hpm_clock_drv.h"
#include "hpm_soc.h"
#include "hpm_soc_feature.h"
#include "hpm_uart_drv.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

extern INT32 UartPutc(INT32 c, VOID *file);

extern VOID UartInit(VOID);
extern INT32 UartGetc(VOID);
extern VOID Uart0RxIrqRegister(VOID);

extern EVENT_CB_S g_shellInputEvent;

#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cplusplus */
#endif /* __cplusplus */
#endif
