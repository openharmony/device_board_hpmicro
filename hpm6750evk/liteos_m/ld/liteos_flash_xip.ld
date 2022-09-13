/*
 * Copyright 2021 hpmicro
 * SPDX-License-Identifier: BSD-3-Clause
 */

ENTRY(_start)

STACK_SIZE = DEFINED(_stack_size) ? _stack_size : 0x4000;
HEAP_SIZE = DEFINED(_heap_size) ? _heap_size : 0x40000;

MEMORY
{
    XPI0 (rx) : ORIGIN = 0x80000000, LENGTH = 16M
    ILM (wx) : ORIGIN = 0, LENGTH = 256K
    DLM (wx) : ORIGIN = 0x80000, LENGTH = 256K
    AXI_SRAM (wx) : ORIGIN = 0x1080000, LENGTH = 768K
    AXI_SRAM_NONCACHEABLE (wx) : ORIGIN = 0x01140000, LENGTH = 256K
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

    .text (__vector_load_addr__ + __vector_ram_end__ - __vector_ram_start__): {
        . = ALIGN(8);
        *(.text)
        *(.text*)
        *(.rodata)
        *(.rodata*)
        *(.srodata)
        *(.srodata*)
        *(.hash)
        *(.dyn*)
        *(.gnu*)
        *(.pl*)

        KEEP(*(.eh_frame))
        *(.eh_frame*)

        KEEP (*(.init))
        KEEP (*(.fini))
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

    __vector_load_addr__ = ADDR(.start) + SIZEOF(.start);
    .vectors ORIGIN(ILM) : AT(__vector_load_addr__) {
        . = ALIGN(8);
        __vector_ram_start__ = .;
        KEEP(*(.interrupt.HalTrapVector.text));
        KEEP(*(.interrupt.text))
        KEEP(*(.vector_table))
        KEEP(*(.isr_vector))
        . = ALIGN(8);
        __vector_ram_end__ = .;
    } > ILM

    .data : AT(etext) {
        . = ALIGN(8);
        __data_start__ = .;
        __global_pointer$ = . + 0x800;
        *(.data)
        *(.data*)
        *(.sdata)
        *(.sdata*)
        *(.tdata)
        *(.tdata*)

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
    } > AXI_SRAM

    .fast : AT(etext + __data_end__ - __data_start__) {
        . = ALIGN(8);
        __ramfunc_start__ = .;
        *(.fast)
        . = ALIGN(8);
        __ramfunc_end__ = .;
    } > AXI_SRAM

    __fw_size__ = __ramfunc_end__ - __ramfunc_start__ + __data_end__ - __data_start__ + etext - __app_load_addr__;
    .bss : {
        . = ALIGN(8);
        __bss_start__ = .;
        *(.bss)
        *(.bss*)
        *(.tbss*)
        *(.sbss*)
        *(.scommon)
        *(.scommon*)
        *(.tcommon*)
        *(.dynsbss*)
        *(COMMON)
        . = ALIGN(8);
        _end = .;
        __bss_end__ = .;
    } > DLM

    .heap : {
        . = ALIGN(8);
        __heap_start = .;
        __heap_start__ = .;
        . += HEAP_SIZE;
        __heap_end__ = .;
        __heap_end = .;
    } > AXI_SRAM

    __heap_size = HEAP_SIZE;
    
    .framebuffer (NOLOAD) : {
        . = ALIGN(8);
        KEEP(*(.framebuffer))
        . = ALIGN(8);
    } > AXI_SRAM

    .noncacheable : AT(etext + __data_end__ - __data_start__ + __ramfunc_end__ - __ramfunc_start__){
        . = ALIGN(8);
        __noncacheable_init_start__ = .;
        KEEP(*(.noncacheable.init))
        __noncacheable_init_end__ = .;
        KEEP(*(.noncacheable))
        __noncacheable_bss_start__ = .;
        KEEP(*(.noncacheable.bss))
        __noncacheable_bss_end__ = .;
        . = ALIGN(8);
    } > AXI_SRAM_NONCACHEABLE

    .stack : {
        . = ALIGN(8);
        __stack_bootom = .;
        . += STACK_SIZE;
        __except_stack_top = .;
        __stack_base__ = .;
        . += STACK_SIZE;
        . = ALIGN(8);
        PROVIDE (_stack = .);
        PROVIDE (_stack_safe = .);
        __start_and_irq_stack_top = .;
    } > DLM

    __noncacheable_start__ = ORIGIN(AXI_SRAM_NONCACHEABLE);
    __noncacheable_end__ = ORIGIN(AXI_SRAM_NONCACHEABLE) + LENGTH(AXI_SRAM_NONCACHEABLE);
}
