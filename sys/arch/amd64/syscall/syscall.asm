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
global syscall_int_handler

; Syscall interrupt handler for int 0x80
syscall_int_handler:
    cli
    push 0x0        ; Push dummy error code (syscalls don't have error codes)
    push 0x80       ; Push interrupt number (0x80 for syscalls)

    ; Save all registers (push order must be the reverse of registers_t
    ; field order so that the layout matches the structure exactly).
    ; registers_t fields order (top -> bottom when on stack):
    ;   r15 r14 r13 r12 r11 r10 r9 r8 rdi rsi rbp rbx rdx rcx rax int_no err_code
    ; Therefore we push from bottom-most field upwards:
    push rax
    push rcx
    push rdx
    push rbx
    push rbp
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    ; Call C syscall handler
    ; The stack pointer now points to our registers_t structure
    ; NOTE: We can't use the syscalls.handler() because the struct
    ; (psuedo-namespace) is not avaliable in the x86 ASM code.
    mov rdi, rsp
    extern syscall_handler
    call syscall_handler

    ; Restore all registers (reverse order of pushes)
    ; The syscall return value is already in rax from the handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rbp
    pop rbx
    pop rdx
    pop rcx
    ; Don't pop rax here -- it contains the return value from syscall_handler
    add rsp, 8      ; Skip rax on stack

    ; Remove error code and interrupt number from stack
    add rsp, 16

    ; Return from interrupt
    sti             ; Re-enable interrupts
    iretq
