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
global enter_userspace

; void enter_userspace(uint64_t rip, uint64_t rsp)
; RDI = user RIP
; RSI = user RSP
enter_userspace:
    cli  ; Disable interrupts during transition
    
    ; Save user RIP and RSP
    mov rax, rdi  ; User RIP
    mov rbx, rsi  ; User RSP
    
    ; Set up segments for user mode (ring 3)
    ; GDT entries: 0x08 = kernel code, 0x10 = kernel data
    ;              0x18 = user code, 0x20 = user data
    ; For user mode we need: 0x23 = user data (0x20 | 3), 0x2B = user code (0x28 | 3)
    
    ; Set up data segments
    mov cx, 0x23        ; User data segment with RPL=3
    mov ds, cx
    mov es, cx
    mov fs, cx
    mov gs, cx
    
    ; Push stack frame for iretq
    ; Stack frame (pushed in reverse order):
    ; SS (stack segment)
    ; RSP (stack pointer)
    ; RFLAGS (flags register)
    ; CS (code segment)
    ; RIP (instruction pointer)
    
    push 0x23           ; SS (user data segment)
    push rbx            ; RSP (user stack pointer)
    pushfq              ; RFLAGS
    pop rcx
    or rcx, 0x200       ; Enable interrupts (IF flag)
    push rcx            ; Push modified RFLAGS
    push 0x2B           ; CS (user code segment with RPL=3)
    push rax            ; RIP (user entry point)
    
    ; Clear all general purpose registers for security
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor rsi, rsi
    xor rdi, rdi
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15
    xor rbp, rbp
    
    ; Jump to user mode (this will pop all the values we pushed)
    iretq
    
    ; Should never reach here
    cli
    hlt