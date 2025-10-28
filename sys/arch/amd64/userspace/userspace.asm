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
[BITS 64]
DEFAULT REL

global enter_userspace

; void enter_userspace(uint64_t rip, uint64_t rsp)
; RDI = user RIP
; RSI = user RSP
enter_userspace:
    cli
    
    ; Save arguments to non-volatile registers
    mov r10, rdi
    mov r11, rsi
    
    ; Build iretq frame - push SS
    mov rax, 0x23
    push rax
    
    ; Push user RSP
    push r11
    
    ; Push RFLAGS with IF set
    pushfq
    pop rax
    or rax, 0x200
    push rax
    
    ; Push CS  
    mov rax, 0x1B
    push rax
    
    ; Push RIP
    push r10
    
    iretq
    
    cli
    hlt