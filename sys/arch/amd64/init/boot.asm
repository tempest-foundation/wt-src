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
; ============================================================================
; Multiboot2 Header
; ============================================================================
section .multiboot_header
align 8
header_start:
    dd 0xE85250D6                    ; Multiboot2 magic number
    dd 0                             ; Architecture: 0 = i386 (protected mode)
    dd header_end - header_start     ; Header length
    ; Checksum: -(magic + arch + length)
    dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start))

    ; Framebuffer tag - Request specific video mode
    dw 5                             ; Type: Framebuffer
    dw 0                             ; Flags: Required
    dd 20                            ; Size: Type(2) + Flags(2) + Size(4) + Width(4) + Height(4) + Depth(4)
    dd 1024                          ; Width in pixels
    dd 768                           ; Height in pixels
    dd 32                            ; Depth in bits per pixel

    ; Padding for 8-byte alignment (tags must be 8-byte aligned)
    dd 0

    ; End tag - Marks end of Multiboot2 header
    dw 0                             ; Type: End
    dw 0                             ; Flags
    dd 8                             ; Size

header_end:

; ============================================================================
; Boot Entry Point (32-bit Protected Mode)
; ============================================================================
section .text
bits 32
global _start
_start:
    ; EBX contains pointer to Multiboot information structure
    ; Save it for later use in kernel
    mov ebp, ebx

    ; Set up page tables for Long Mode
    call setup_page_tables
    
    ; Enable required CPU features
    call enable_cpu_features
    
    ; Switch to Long Mode
    call enter_long_mode

    ; Load 64-bit GDT and jump to 64-bit code
    lgdt [gdt_pointer]
    jmp 0x08:long_mode

; ============================================================================
; setup_page_tables - Initialize page table hierarchy for 4GB identity mapping
; ============================================================================
setup_page_tables:
    ; Clear all page tables to zero
    mov edi, pml4_table
    mov ecx, 4096 * 6                ; Clear 6 page tables (24KB total)
    xor eax, eax
    rep stosb

    ; Set up PML4 (Page Map Level 4) - top level
    ; Map both low memory (0x0000000000000000) and kernel space (0xFFFF800000000000)
    mov eax, pml3_table
    or eax, 0b11                     ; Present + Writable flags
    mov [pml4_table], eax            ; PML4[0] -> PML3 (low memory)
    mov [pml4_table + 256*8], eax    ; PML4[256] -> PML3 (high half kernel)

    ; Set up PML3 (Page Directory Pointer) - second level
    ; Each PML3 entry covers 1 GiB, we need 4 entries for 4 GiB
    mov eax, pml2_table_0
    or eax, 0b11
    mov [pml3_table + 0*8], eax      ; First 1 GiB
    
    mov eax, pml2_table_1
    or eax, 0b11
    mov [pml3_table + 1*8], eax      ; Second 1 GiB
    
    mov eax, pml2_table_2
    or eax, 0b11
    mov [pml3_table + 2*8], eax      ; Third 1 GiB
    
    mov eax, pml2_table_3
    or eax, 0b11
    mov [pml3_table + 3*8], eax      ; Fourth 1 GiB

    ; Set up PML2 (Page Directory) - third level
    ; Use 2 MiB huge pages (no PML1 needed)
    ; Map 2048 pages × 2 MiB = 4 GiB
    call map_4gib_identity
    
    ret

; ============================================================================
; map_4gib_identity - Map 4 GiB using 2 MiB huge pages
; ============================================================================
map_4gib_identity:
    mov ecx, 0                       ; Counter: 0 to 2047
.map_loop:
    ; Calculate physical address for this page
    mov eax, ecx
    imul eax, 0x200000               ; Physical address = ECX × 2 MiB
    or eax, 0b10000011               ; Present + Writable + Huge Page (PS bit)
    
    ; Determine which PML2 table to use
    mov edx, ecx
    shr edx, 9                       ; EDX = table index (0..3) = ECX / 512
    mov esi, ecx
    and esi, 0x1FF                   ; ESI = entry index (0..511) = ECX % 512

    ; Select the appropriate PML2 table
    lea edi, [pml2_table_0 + esi*8]  ; Default: table 0
    cmp edx, 1
    je .use_table_1
    cmp edx, 2
    je .use_table_2
    cmp edx, 3
    je .use_table_3
    jmp .write_entry

.use_table_1:
    lea edi, [pml2_table_1 + esi*8]
    jmp .write_entry
.use_table_2:
    lea edi, [pml2_table_2 + esi*8]
    jmp .write_entry
.use_table_3:
    lea edi, [pml2_table_3 + esi*8]

.write_entry:
    mov [edi], eax                   ; Write PML2 entry
    
    inc ecx
    cmp ecx, 2048                    ; Map all 2048 pages
    jl .map_loop
    
    ret

; ============================================================================
; enable_cpu_features - Enable PAE and SSE
; ============================================================================
enable_cpu_features:
    mov eax, cr4
    or eax, (1 << 5)                 ; PAE (Physical Address Extension)
    or eax, (1 << 9)                 ; OSFXSR (OS support for FXSAVE/FXRSTOR)
    or eax, (1 << 10)                ; OSXMMEXCPT (OS support for unmasked SIMD exceptions)
    mov cr4, eax

    ; Load PML4 address into CR3 (page table base register)
    mov eax, pml4_table
    mov cr3, eax
    
    ret

; ============================================================================
; enter_long_mode - Enable Long Mode and paging
; ============================================================================
enter_long_mode:
    ; Enable Long Mode by setting LME bit in EFER MSR
    mov ecx, 0xC0000080              ; EFER MSR (Extended Feature Enable Register)
    rdmsr                            ; Read current value
    or eax, (1 << 8)                 ; Set LME (Long Mode Enable) bit
    wrmsr                            ; Write back

    ; Enable paging (which activates Long Mode)
    mov eax, cr0
    or eax, (1 << 31)                ; PG (Paging)
    or eax, (1 << 1)                 ; MP (Monitor Coprocessor)
    mov cr0, eax
    
    ret

; ============================================================================
; BSS Section - Uninitialized Data
; ============================================================================
section .bss
align 4096
pml4_table:      resb 4096           ; Level 4: 512 entries × 8 bytes
pml3_table:      resb 4096           ; Level 3: 512 entries × 8 bytes
pml2_table_0:    resb 4096           ; Level 2 table 0: covers 0-1 GiB
pml2_table_1:    resb 4096           ; Level 2 table 1: covers 1-2 GiB
pml2_table_2:    resb 4096           ; Level 2 table 2: covers 2-3 GiB
pml2_table_3:    resb 4096           ; Level 2 table 3: covers 3-4 GiB
stack_bottom:    resb 16384          ; 16 KB stack
stack_top:

; ============================================================================
; Global Descriptor Table (GDT) for Long Mode
; ============================================================================
section .rodata
align 8
gdt:
    dq 0                             ; Null descriptor (required)
    ; 64-bit code segment descriptor
    dq (1 << 43) | (1 << 44) | (1 << 47) | (1 << 53)
    ; Bits: Executable, Code/Data, Present, Long mode

gdt_pointer:
    dw $ - gdt - 1                   ; GDT limit (size - 1)
    dq gdt                           ; GDT base address

; ============================================================================
; Long Mode Entry Point (64-bit)
; ============================================================================
section .text
bits 64
long_mode:
    ; Clear segment registers (not used in Long Mode except for FS/GS)
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax

    ; Set up stack pointer
    mov rsp, stack_top

    ; Pass Multiboot information pointer to kernel
    ; RDI = first argument in System V AMD64 ABI
    mov rdi, rbp

    ; Call C++ kernel entry point
    extern start_kernel
    call start_kernel

    ; If kernel returns, halt the CPU
.hang:
    cli                              ; Disable interrupts
    hlt                              ; Halt CPU
    jmp .hang                        ; Loop forever if NMI wakes us