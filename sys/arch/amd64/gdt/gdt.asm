; SPDX-License-Identifier: GPL-3.0-only
;
; -- BEGIN METADATA HEADER --
; <*---The Wind/Tempest Project---*>
; 
; Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
; Maintainer : Tempest Foundation <development@tempestfoundation.org>
; 
; Copyright (c) Tempest Foundation, 2025
; -- END OF METADATA HEADER --
;
section .data
align 16
gdt_start:
    ; Null descriptor (required)
    dq 0x0000000000000000

    ; Kernel code segment (0x08)
    dq 0x00AF9A000000FFFF  ; Code, Ring 0, 64-bit
    
    ; Kernel data segment (0x10)
    dq 0x00AF92000000FFFF  ; Data, Ring 0, 64-bit
    
    ; User code segment (0x18 + 3 = 0x1B)
    dq 0x00AFFA000000FFFF  ; Code, Ring 3, 64-bit
    
    ; User data segment (0x20 + 3 = 0x23)
    dq 0x00AFF2000000FFFF  ; Data, Ring 3, 64-bit
    
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dq gdt_start                 ; Offset

section .text
global load_gdt
load_gdt:
    lgdt [gdt_descriptor]
    
    ; Reload segment registers
    mov ax, 0x10        ; Kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Far return to reload CS
    push 0x08           ; Kernel code segment
    lea rax, [rel .reload_cs]
    push rax
    retfq
    
.reload_cs:
    ret