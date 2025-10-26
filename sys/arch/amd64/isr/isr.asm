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
global isr0, isr1, isr2, isr3, isr4, isr5, isr6, isr7
global isr8, isr9, isr10, isr11, isr12, isr13, isr14, isr15
global isr16, isr17, isr18, isr19, isr20, isr21, isr22, isr23
global isr24, isr25, isr26, isr27, isr28, isr29, isr30, isr31

%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    cli
    push 0      ; Dummy error code for consistent stack layout
    push %1     ; Interrupt number
    jmp isr_common_stub
%endmacro

%macro ISR_ERRCODE 1
global isr%1
isr%1:
    cli
    push %1     ; Interrupt number (error code already on stack from CPU)
    jmp isr_common_stub
%endmacro

; ============================================================================
; ISR Definitions --- CPU Exception Handlers (Interrupts 0-31)
; ============================================================================
; Exceptions without error codes
ISR_NOERRCODE 0     ; Division by Zero
ISR_NOERRCODE 1     ; Debug
ISR_NOERRCODE 2     ; Non-Maskable Interrupt
ISR_NOERRCODE 3     ; Breakpoint
ISR_NOERRCODE 4     ; Overflow
ISR_NOERRCODE 5     ; Bound Range Exceeded
ISR_NOERRCODE 6     ; Invalid Opcode
ISR_NOERRCODE 7     ; Device Not Available
ISR_ERRCODE   8     ; Double Fault
ISR_NOERRCODE 9     ; Coprocessor Segment Overrun (legacy)
ISR_ERRCODE   10    ; Invalid TSS
ISR_ERRCODE   11    ; Segment Not Present
ISR_ERRCODE   12    ; Stack Segment Fault
ISR_ERRCODE   13    ; General Protection Fault
ISR_ERRCODE   14    ; Page Fault
ISR_NOERRCODE 15    ; Reserved
ISR_NOERRCODE 16    ; x87 Floating-Point Exception
ISR_ERRCODE   17    ; Alignment Check
ISR_NOERRCODE 18    ; Machine Check
ISR_NOERRCODE 19    ; SIMD Floating-Point Exception
ISR_NOERRCODE 20    ; Virtualization Exception
ISR_NOERRCODE 21    ; Control Protection Exception
ISR_NOERRCODE 22    ; Reserved
ISR_NOERRCODE 23    ; Reserved
ISR_NOERRCODE 24    ; Reserved
ISR_NOERRCODE 25    ; Reserved
ISR_NOERRCODE 26    ; Reserved
ISR_NOERRCODE 27    ; Reserved
ISR_NOERRCODE 28    ; Hypervisor Injection Exception
ISR_NOERRCODE 29    ; VMM Communication Exception
ISR_ERRCODE   30    ; Security Exception
ISR_NOERRCODE 31    ; Reserved

; ============================================================================
; isr_common_stub --- Common interrupt service routine handler
; ============================================================================
; Called by all ISR stubs after they push interrupt number and error code.
; 
; Behavior:
;   1. Saves all general-purpose registers to create registers_t structure
;   2. Calls C-level isr_handler with pointer to registers
;   3. Restores all registers
;   4. Cleans up stack (removes error code and interrupt number)
;   5. Returns from interrupt
;
; Stack layout when entering this stub:
;   [rsp + 8]  = interrupt number
;   [rsp + 0]  = error code
;
; Stack layout after saving all registers (registers_t structure):
;   [rsp + 136] = (original stack location)
;   [rsp + 128] = interrupt number
;   [rsp + 120] = error code  
;   [rsp + 112] = rax
;   [rsp + 104] = rcx
;   [rsp + 96]  = rdx
;   [rsp + 88]  = rbx
;   [rsp + 80]  = rbp
;   [rsp + 72]  = rsi
;   [rsp + 64]  = rdi
;   [rsp + 56]  = r8
;   [rsp + 48]  = r9
;   [rsp + 40]  = r10
;   [rsp + 32]  = r11
;   [rsp + 24]  = r12
;   [rsp + 16]  = r13
;   [rsp + 8]   = r14
;   [rsp + 0]   = r15  <-- rsp points here
; ============================================================================
extern isr_handler
isr_common_stub:
    ; Save all general-purpose registers
    ; Push order matches registers_t structure layout (see registers.h)
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
    
    ; Call C handler with pointer to register structure
    ; rdi = first argument in System V AMD64 ABI
    mov rdi, rsp
    call isr_handler
    
    ; Restore all general-purpose registers (reverse order)
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
    pop rax
    
    ; Remove error code and interrupt number from stack
    add rsp, 16
    
    ; Return from interrupt (pops CS, RIP, RFLAGS, SS, RSP)
    iretq