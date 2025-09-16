/* SPDX-License-Identifier: LSL-2.0 */
/*
 * -- BEGIN LICENSE HEADER --
 * The Wind/Tempest Project
 *
 * File:        sys/arch/amd64/linker.ld
 * Author(s):   Russian95 <russian95@tempestfoundation.org>
 *              (https://github.com/Russian95CrE)
 * Maintainer:  Tempest Foundation <development@tempestfoundation.org>
 * Link:        https://wtsrc.tempestfoundation.org
 *
 * Copyright (C) 2025 Tempest Foundation
 * Licensed under the Liberty Software License, Version 2.0
 * -- END OF LICENSE HEADER --
 */
ENTRY(_start)

SECTIONS
{
    /* The kernel starts at 1MB */
    . = 1M;

    /* The first section must be the multiboot header */
    .text : ALIGN(4K) {
        KEEP(*(.multiboot_header))
        *(.text .text.*)
    }

    .rodata : ALIGN(4K) {
        *(.rodata .rodata.*)
    }

    .data : ALIGN(4K) {
        *(.data .data.*)
    }

    .bss : ALIGN(4K) {
        *(.bss .bss.*)
        *(COMMON)
    }

    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}
