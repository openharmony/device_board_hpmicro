/*
 * Copyright (c) 2021-2023 HPMicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = DEFINED(_stack_size) ? _stack_size : 0x4000;
HEAP_SIZE = DEFINED(_heap_size) ? _heap_size : 0x40000;

MEMORY
{
    XPI0 (rx) : ORIGIN = 0x80000000, LENGTH = _flash_size
    ILM (wx) : ORIGIN = 0x00000000, LENGTH = 256K
    DLM (w) : ORIGIN = 0x00080000, LENGTH = 256K
    AXI_SRAM (wx) : ORIGIN = 0x01200000, LENGTH = 256K
    NONCACHEABLE_RAM (w) : ORIGIN = 0x01240000, LENGTH = 256K
    AHB_SRAM (w) : ORIGIN = 0xF0400000, LENGTH = 32k
    APB_SRAM (w): ORIGIN = 0xF4130000, LENGTH = 16k
}

__nor_cfg_option_load_addr__ = ORIGIN(XPI0) + 0x400;
__boot_header_load_addr__ = ORIGIN(XPI0) + 0x1000;
__app_load_addr__ = ORIGIN(XPI0) + 0x3000;
__boot_header_length__ = __boot_header_end__ - __boot_header_start__;
__app_offset__ = __app_load_addr__ - __boot_header_load_addr__;


SECTIONS
{
    .nor_cfg_option __nor_cfg_option_load_addr__ : {
        KEEP(*(.nor_cfg_option))
    } > XPI0

    .boot_header __boot_header_load_addr__ : {
        __boot_header_start__ = .;
        KEEP(*(.boot_header))
        KEEP(*(.fw_info_table))
        KEEP(*(.dc_info))
        __boot_header_end__ = .;
    } > XPI0

    .start __app_load_addr__ : {
        . = ALIGN(8);
        KEEP(*(.start))
    } > XPI0

    __vector_load_addr__ = ADDR(.start) + SIZEOF(.start);
    .vectors ORIGIN(ILM) : AT(__vector_load_addr__) {
        . = ALIGN(8);
        __vector_ram_start__ = .;
        /* For OHOS Start */
        KEEP(*(.interrupt.HalTrapVector.text));
        KEEP(*(.interrupt.text))
        /* For OHOS End */
    
        KEEP(*(.vector_table))
        KEEP(*(.isr_vector))
        . = ALIGN(8);
        __vector_ram_end__ = .;
    } > ILM

    .text (__vector_load_addr__ + SIZEOF(.vectors)) : {
        . = ALIGN(8);
        *(EXCLUDE_FILE (nx*.o*) .text)
        *(EXCLUDE_FILE (nx*.o*) .text*)
        *(EXCLUDE_FILE (nx*.o*) .rodata)
        *(EXCLUDE_FILE (nx*.o*) .rodata*)
        *(EXCLUDE_FILE (nx*.o*) .srodata)
        *(EXCLUDE_FILE (nx*.o*) .srodata*)

        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)

        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))

        /* section information for usbh class */
        . = ALIGN(8);
        __usbh_class_info_start__ = .;
        KEEP(*(.usbh_class_info))
        __usbh_class_info_end__ = .;

        /* RT-Thread related sections - Start */
        /* section information for finsh shell */
        . = ALIGN(4);
        __fsymtab_start = .;
        KEEP(*(FSymTab))
        __fsymtab_end = .;
        . = ALIGN(4);
        __vsymtab_start = .;
        KEEP(*(VSymTab))
        __vsymtab_end = .;
        . = ALIGN(4);

        . = ALIGN(4);
        __rt_init_start = .;
        KEEP(*(SORT(.rti_fn*)))
        __rt_init_end = .;
        . = ALIGN(4);

        /* section information for modules */
        . = ALIGN(4);
        __rtmsymtab_start = .;
        KEEP(*(RTMSymTab))
        __rtmsymtab_end = .;

        /* RT-Thread related sections - end */
        . = ALIGN(8);

        /* For OHOS Start */
        _hdf_drivers_start = .;
        KEEP(*(.hdf.driver))
        _hdf_drivers_end = .; /* define a section for hdf driver */

         __zinitcall_bsp_start = .;
        KEEP (*(.zinitcall.bsp0.init))
        KEEP (*(.zinitcall.bsp1.init))
        KEEP (*(.zinitcall.bsp2.init))
        KEEP (*(.zinitcall.bsp3.init))
        KEEP (*(.zinitcall.bsp4.init))
        __zinitcall_bsp_end = .;
        . = ALIGN(4);
        __zinitcall_device_start = .;
        KEEP (*(.zinitcall.device0.init))
        KEEP (*(.zinitcall.device1.init))
        KEEP (*(.zinitcall.device2.init))
        KEEP (*(.zinitcall.device3.init))
        KEEP (*(.zinitcall.device4.init))
        __zinitcall_device_end = .;
        . = ALIGN(4);
        __zinitcall_core_start = .;
        KEEP (*(.zinitcall.core0.init))
        KEEP (*(.zinitcall.core1.init))
        KEEP (*(.zinitcall.core2.init))
        KEEP (*(.zinitcall.core3.init))
        KEEP (*(.zinitcall.core4.init))
        __zinitcall_core_end = .;
        . = ALIGN(4);
        __zinitcall_sys_service_start = .;
        KEEP (*(.zinitcall.sys.service0.init))
        KEEP (*(.zinitcall.sys.service1.init))
        KEEP (*(.zinitcall.sys.service2.init))
        KEEP (*(.zinitcall.sys.service3.init))
        KEEP (*(.zinitcall.sys.service4.init))
        __zinitcall_sys_service_end = .;
        . = ALIGN(4);
        __zinitcall_sys_feature_start = .;
        KEEP (*(.zinitcall.sys.feature0.init))
        KEEP (*(.zinitcall.sys.feature1.init))
        KEEP (*(.zinitcall.sys.feature2.init))
        KEEP (*(.zinitcall.sys.feature3.init))
        KEEP (*(.zinitcall.sys.feature4.init))
        __zinitcall_sys_feature_end = .;
        . = ALIGN(4);
        __zinitcall_run_start = .;
        KEEP (*(.zinitcall.run0.init))
        KEEP (*(.zinitcall.run1.init))
        KEEP (*(.zinitcall.run2.init))
        KEEP (*(.zinitcall.run3.init))
        KEEP (*(.zinitcall.run4.init))
        __zinitcall_run_end = .;
        . = ALIGN(4);
        __zinitcall_app_service_start = .;
        KEEP (*(.zinitcall.app.service0.init))
        KEEP (*(.zinitcall.app.service1.init))
        KEEP (*(.zinitcall.app.service2.init))
        KEEP (*(.zinitcall.app.service3.init))
        KEEP (*(.zinitcall.app.service4.init))
        __zinitcall_app_service_end = .;
        . = ALIGN(4);
        __zinitcall_app_feature_start = .;
        KEEP (*(.zinitcall.app.feature0.init))
        KEEP (*(.zinitcall.app.feature1.init))
        KEEP (*(.zinitcall.app.feature2.init))
        KEEP (*(.zinitcall.app.feature3.init))
        KEEP (*(.zinitcall.app.feature4.init))
        __zinitcall_app_feature_end = .;
        . = ALIGN(4);
        __zinitcall_test_start = .;
        KEEP (*(.zinitcall.test0.init))
        KEEP (*(.zinitcall.test1.init))
        KEEP (*(.zinitcall.test2.init))
        KEEP (*(.zinitcall.test3.init))
        KEEP (*(.zinitcall.test4.init))
        __zinitcall_test_end = .;
        . = ALIGN(4);
        __zinitcall_exit_start = .;
        KEEP (*(.zinitcall.exit0.init))
        KEEP (*(.zinitcall.exit1.init))
        KEEP (*(.zinitcall.exit2.init))
        KEEP (*(.zinitcall.exit3.init))
        KEEP (*(.zinitcall.exit4.init))
        __zinitcall_exit_end = .;
        . = ALIGN(4);
        /* For OHOS End */
    } > XPI0

    .rel : {
        KEEP(*(.rel*))
    } > XPI0

    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    __data_load_addr__ = etext;
    .data : AT(__data_load_addr__) {
        . = ALIGN(8);
        __data_start__ = .;
        __global_pointer$ = . + 0x800;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)

        KEEP(*(.jcr))
        KEEP(*(.dynamic))
        KEEP(*(.got*))
        KEEP(*(.got))
        KEEP(*(.gcc_except_table))
        KEEP(*(.gcc_except_table.*))

        . = ALIGN(8);
        PROVIDE(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE(__preinit_array_end = .);

        . = ALIGN(8);
        PROVIDE(__init_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(__init_array_end = .);

        . = ALIGN(8);
        PROVIDE(__finit_array_start = .);
        KEEP(*(SORT_BY_INIT_PRIORITY(.finit_array.*)))
        KEEP(*(.finit_array))
        PROVIDE(__finit_array_end = .);

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.ctors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .ctors))
        KEEP(*(SORT(.ctors.*)))
        KEEP(*(.ctors))

        . = ALIGN(8);
        KEEP(*crtbegin*.o(.dtors))
        KEEP(*(EXCLUDE_FILE (*crtend*.o) .dtors))
        KEEP(*(SORT(.dtors.*)))
        KEEP(*(.dtors))
        . = ALIGN(8);
        __data_end__ = .;
        PROVIDE (__edata = .);
        PROVIDE (_edata = .);
        PROVIDE (edata = .);
    } > DLM

    __fast_load_addr__ = etext + SIZEOF(.data);
    .fast : AT(__fast_load_addr__) {
        . = ALIGN(8);
        PROVIDE(__ramfunc_start__ = .);
        *(.fast)
        *(.fast.*)
        nx*.o*(.text)
        nx*.o*(.text*)
        nx*.o*(.rodata)
        nx*.o*(.rodata*)
        nx*.o*(.srodata)
        nx*.o*(.srodata*)
        . = ALIGN(8);
        PROVIDE(__ramfunc_end__ = .);
    } > ILM

    .bss (NOLOAD) : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.sbss*)
        *(.scommon)
        *(.scommon*)
        *(.dynsbss*)
        *(COMMON)
        . = ALIGN(8);
        _end = .;
        __bss_end__ = .;
    } > DLM

    .tbss (NOLOAD) : {
        . = ALIGN(8);
        PROVIDE(__tbss_start__ = .);
        __thread_pointer$ = .;
        *(.tbss)
        *(.tbss.*)
        *(.gnu.linkonce.tb.*)
        *(.tcommon)
        . = ALIGN(8);
        PROVIDE(__tbss_end__ = .);
    } > AXI_SRAM

    __tdata_load_addr__ = etext + SIZEOF(.data) + SIZEOF(.fast);
    .tdata : AT(__tdata_load_addr__) {
        . = ALIGN(8);
        PROVIDE(__tdata_start__ = .);
        *(.tdata)
        *(.tdata.*)
        *(.gnu.linkonce.td.*)
        . = ALIGN(8);
        PROVIDE(__tdata_end__ = .);
    } > AXI_SRAM

    .framebuffer (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.framebuffer))
        . = ALIGN(8);
    } > AXI_SRAM

    __noncacheable_init_load_addr__ = etext + SIZEOF(.data) + SIZEOF(.fast) + SIZEOF(.tdata);
    .noncacheable.init : AT(__noncacheable_init_load_addr__) {
        . = ALIGN(8);
        __noncacheable_init_start__ = .;
        KEEP(*(.noncacheable.init))
        __noncacheable_init_end__ = .;
        . = ALIGN(8);
    } > NONCACHEABLE_RAM

    .noncacheable.bss (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.noncacheable))
        __noncacheable_bss_start__ = .;
        KEEP(*(.noncacheable.bss))
        __noncacheable_bss_end__ = .;
        . = ALIGN(8);
    } > NONCACHEABLE_RAM

    .ahb_sram (NOLOAD) : {
        KEEP(*(.ahb_sram))
    } > AHB_SRAM

    .apb_sram (NOLOAD) : {
        KEEP(*(.backup_sram))
    } > APB_SRAM

    .fast_ram (NOLOAD) : {
        KEEP(*(.fast_ram))
    } > DLM

    .heap (NOLOAD) : {
        . = ALIGN(8);
        __heap_start = .; /* Only for OHOS Start*/
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;
        __heap_end = .; /* Only for OHOS  End */
    } > AXI_SRAM

    __heap_size = HEAP_SIZE;

    .stack (NOLOAD) : {
        . = ALIGN(8);
        __stack_bootom = .; /* Only for OHOS  */
        . += STACK_SIZE; /* Only for OHOS  */
        __except_stack_top = .; /* Only for OHOS  */

        __stack_base__ = .;
        . += STACK_SIZE;
        . = ALIGN(8);
        PROVIDE (_stack = .);
        PROVIDE (_stack_safe = .);
        __start_and_irq_stack_top = .; /* Only for OHOS  */
    } > DLM

    __noncacheable_start__ = ORIGIN(NONCACHEABLE_RAM);
    __noncacheable_end__ = ORIGIN(NONCACHEABLE_RAM) + LENGTH(NONCACHEABLE_RAM);

    __fw_size__ = SIZEOF(.start) + SIZEOF(.vectors) + SIZEOF(.rel) + SIZEOF(.text) + SIZEOF(.data) + SIZEOF(.fast) + SIZEOF(.tdata) + SIZEOF(.noncacheable.init);
    ASSERT(__fw_size__ <= LENGTH(XPI0), "******  FAILED! XPI0 has not enough space!  ******")
}
