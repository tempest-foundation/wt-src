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
    
    ; TSS descriptor (0x28) - 16 bytes in 64-bit mode
tss_descriptor:
    dw tss_end - tss_start - 1  ; Limit (low)
    dw 0                         ; Base (low) -> filled at runtime
    db 0                          ; Base (mid low) -> filled at runtime
    db 0x89                       ; Access: Present, DPL=0, TSS Available
    db 0x00                       ; Flags + Limit (high)
    db 0                          ; Base (mid high) -> filled at runtime
    dd 0                          ; Base (high) -> filled at runtime + Reserved
    dd 0                          ; Reserved
    
gdt_end:

; TSS structure
align 16
tss_start:
    dd 0        ; Reserved
    dq 0        ; RSP0 (will be set at runtime)
    dq 0        ; RSP1
    dq 0        ; RSP2
    dq 0        ; Reserved
    dq 0        ; IST1
    dq 0        ; IST2
    dq 0        ; IST3
    dq 0        ; IST4
    dq 0        ; IST5
    dq 0        ; IST6
    dq 0        ; IST7
    dq 0        ; Reserved
    dw 0        ; Reserved
    dw 0        ; I/O Map Base
tss_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size
    dq gdt_start                 ; Offset

section .text
global load_gdt
load_gdt:
    ; Fix TSS descriptor base address
    lea rax, [rel tss_start]
    mov [rel tss_descriptor + 2], ax    ; Base low
    shr rax, 16
    mov [rel tss_descriptor + 4], al    ; Base mid low
    shr rax, 8
    mov [rel tss_descriptor + 7], al    ; Base mid high
    shr rax, 8
    mov [rel tss_descriptor + 8], eax   ; Base high
    
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
    ; Load TSS
    mov ax, 0x28        ; TSS selector
    ltr ax
    ret

global set_kernel_stack
; void set_kernel_stack(uint64_t rsp)
; RDI = kernel stack pointer
set_kernel_stack:
    lea rax, [rel tss_start]
    mov [rax + 4], rdi  ; Set RSP0 (offset 4 in TSS)
    ret