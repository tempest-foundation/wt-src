# SPDX-License-Identifier: GPL-3-0-only
# * -- BEGIN METADATA HEADER --
# * 
# * <*---The Wind/Tempest Project---*>
# * 
# * Author(s)  : Tempik25 <tempik25@tempestfoundation.org>
# * Maintainer : Tempest Foundation <development@tempestfoundation.org>
# * 
# * Copyright (c) Tempest Foundation, 2025
# * -- END OF METADATA HEADER --

# Build configuration
MODE ?= Release
PLATFORM := amd64

# ==============================================================================
# Tool Configuration
# ==============================================================================
CC := clang++
NASM := nasm
LD := ld
RM := rm -rf
STRIP := strip
XORRISO := xorriso
MKDIR := mkdir -p

# ==============================================================================
# Directory Structure
# ==============================================================================
SRC_DIR := sys
INCLUDE_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
OUT_DIR := $(BUILD_DIR)/out
ISO_DIR := $(BUILD_DIR)/isodir
BOOT_DIR := $(ISO_DIR)/boot

# ==============================================================================
# Limine Bootloader
# ==============================================================================
LIMINE_DIR := stand/limine
LIMINE_CFG := $(LIMINE_DIR)/limine.conf

LIMINE_FILES := \
	$(LIMINE_DIR)/limine-bios.sys \
	$(LIMINE_DIR)/limine-bios-cd.bin \
	$(LIMINE_DIR)/limine-uefi-cd.bin \
	$(LIMINE_DIR)/BOOTX64.EFI

# ==============================================================================
# Output Files
# ==============================================================================
KERNEL_ELF := $(OUT_DIR)/wt.elf
KERNEL_BOOT := $(BOOT_DIR)/wt.elf
ISO_FILE := $(OUT_DIR)/wt.iso
DISK_FILE := $(BUILD_DIR)/hard-drive.img

# ==============================================================================
# Compiler Flags
# ==============================================================================
# Base C++ flags (common to all modes)
BASE_CFLAGS := \
	-std=c++20 \
	-ffreestanding \
	-fno-pie \
	-fno-stack-protector \
	-m64 \
	$(shell find $(INCLUDE_DIR) -type d -printf '-I%p\n') \
	-I$(SRC_DIR)

# Warning flags
WARNING_CFLAGS := \
	-Wall \
	-Wextra \
	-Wpedantic \
	-Wconversion \
	-Werror \
	-Wundef \
	-Wshadow \
	-Wno-unused-command-line-argument

# Optimization flags
OPTIMIZE_CFLAGS := \
	-ffunction-sections \
	-fdata-sections

# Mode-specific flags
ifeq ($(MODE),Debug)
    MODE_CFLAGS := -Og -g1
    STRIP_DEBUG := false
else ifeq ($(MODE),Release)
    # ! Warning! LTO is not supported here!!!
    # * See, most of the kernel is coded on the C programming language. Which
    # * It self, you can use a normal LTO and it will optimize the code with no
    # * problems.
    # ? That should be a great idea to implement also in the x86 assmembly files?
    # ! NO! NO FUCKING WAY, IT WILL BLOW THE WHOLE KERNEL!
    # * This is a normal serial output using LTO in the Linker+CFlags:
    # *		Oops! Your system crashed
    # *
    # *
    # *	 	Oops! Your system crashed
    # * 
    # *
    # *		Oops! Your system crashed
    # ! Don't put native yet, in boot, it will boot-loop with the kernel panic
    # ! like in this example output, if you really want to activate native
    # ! to insert modern instructions, you first need to make it detect and
    # ! activate in boot.
    MODE_CFLAGS := -O3 -march=x86-64 -mtune=generic -fno-omit-frame-pointer
    STRIP_DEBUG := true
else
    $(error Invalid MODE '$(MODE)'. Use 'Debug' or 'Release')
endif

# Linker flags
LDFLAGS := \
	-no-pie \
	-fno-stack-protector \
	-ffreestanding \
	-m64 \
	-nostdinc \
	-nostdlib \
	-Wl,--gc-sections

# Dependency generation flags
DEPFLAGS := -MMD -MP

# Assembly flags
NASMFLAGS := -f elf64

# ==============================================================================
# Arch/Macro
# ==============================================================================
ifeq ($(PLATFORM),amd64)
    ARCH_DEFS := -DARCH_AMD64
    ARCH_INCLUDE := $(SRC_DIR)/arch/amd64/include
else ifeq ($(PLATFORM),arm64)
    ARCH_DEFS := -DARCH_AARCH64
    ARCH_INCLUDE := $(SRC_DIR)/arch/arm64/include
else
    $(error Unknown arch: $(PLATFORM))
endif

CFLAGS := $(BASE_CFLAGS) $(WARNING_CFLAGS) $(OPTIMIZE_CFLAGS) $(MODE_CFLAGS) $(ARCH_DEFS) -I$(ARCH_INCLUDE)

ifneq ($(filter -march=native,$(CFLAGS)),)
    $(error -march=native is NOT supported on Tempest, use -march=x86_64 or other arch)
endif

ifneq ($(filter -flto,$(CFLAGS)),)
    $(error -flto is NOT supported on Tempest)
endif

# ==============================================================================
# Source Files and Objects
# ==============================================================================

C_SOURCES := \
	$(shell \
		find $(SRC_DIR) -type f -name '*.cpp' ! -path "$(SRC_DIR)/arch/*" ; \
		find $(SRC_DIR)/arch/$(PLATFORM) -type f -name '*.cpp' 2>/dev/null \
	)

ASM_SOURCES := \
	$(shell \
		find $(SRC_DIR) -type f -name '*.asm' ! -path "$(SRC_DIR)/arch/*" ; \
		find $(SRC_DIR)/arch/$(PLATFORM) -type f -name '*.asm' 2>/dev/null \
	)

C_OBJECTS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm, $(OBJ_DIR)/%.o, $(ASM_SOURCES))
ALL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS := $(C_OBJECTS:.o=.d)

# Linker script
LINKER_SCRIPT := $(SRC_DIR)/arch/$(PLATFORM)/linker.ld

# ==============================================================================
# QEMU Configuration
# ==============================================================================
QEMU_FLAGS := \
	-cdrom $(ISO_FILE) \
	-vga vmware \
	-cpu host \
	-machine hpet=on \
	-enable-kvm \
	-m 128M \
	-serial mon:stdio \
	-drive file=$(DISK_FILE),format=raw,if=ide \
	-display sdl

QEMU_CMD := qemu-system-x86_64 $(QEMU_FLAGS)

# ==============================================================================
# Build Targets
# ==============================================================================

# Default target
.PHONY: all
all: $(ISO_FILE) $(DISK_FILE)

# ==============================================================================
# Directory Creation
# ==============================================================================
$(OBJ_DIR) $(OUT_DIR) $(BOOT_DIR):
	$(MKDIR) $@

# ==============================================================================
# Object File Generation
# ==============================================================================

# Compile C source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "  CC      $<"
	@$(MKDIR) $(dir $@)
	@$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

# Assemble ASM source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.asm | $(OBJ_DIR)
	@echo "  NASM    $<"
	@$(MKDIR) $(dir $@)
	@$(NASM) $(NASMFLAGS) $< -o $@

# ==============================================================================
# Kernel Binary
# ==============================================================================
$(KERNEL_ELF): $(ALL_OBJECTS) $(LINKER_SCRIPT) | $(OUT_DIR)
	@echo "  LD      $@"
	@$(CC) $(LDFLAGS) -T $(LINKER_SCRIPT) $(ALL_OBJECTS) -o $@
ifeq ($(STRIP_DEBUG),true)
	@echo "  STRIP   $@"
	@$(STRIP) --strip-debug $@
endif

# ==============================================================================
# ISO Generation
# ==============================================================================
$(ISO_FILE): $(KERNEL_ELF) $(LIMINE_FILES) $(LIMINE_CFG) | $(BOOT_DIR)
	@echo "  COPY    kernel to boot directory"
	@cp $(KERNEL_ELF) $(KERNEL_BOOT)
	@echo "  COPY    limine configuration"
	@cp $(LIMINE_CFG) $(ISO_DIR)/
	@echo "  COPY    limine files"
	@cp $(LIMINE_FILES) $(ISO_DIR)/
	@echo "  ISO     $@"
	@$(XORRISO) -as mkisofs \
		-quiet \
		-o $@ \
		-b limine-bios-cd.bin \
		-no-emul-boot \
		-boot-load-size 4 \
		-boot-info-table \
		-isohybrid-mbr $(ISO_DIR)/limine-bios.sys \
		-eltorito-alt-boot \
		-e limine-uefi-cd.bin \
		-no-emul-boot \
		-isohybrid-gpt-basdat \
		$(ISO_DIR) 2>/dev/null

# ==============================================================================
# Disk Image
# ==============================================================================
$(DISK_FILE): tools/create_disk.sh
	@echo "  DISK    $@"
	@./tools/create_disk.sh

# ==============================================================================
# Utility Targets
# ==============================================================================

# Run in QEMU
.PHONY: run
run: all
	@echo "  QEMU    Starting virtual machine..."
	@$(QEMU_CMD)

# Run in debug mode (with QEMU gdb server)
.PHONY: debug
debug: all
	@echo "  QEMU    Starting virtual machine in debug mode..."
	@$(QEMU_CMD) -s -S

# Clean build artifacts
.PHONY: clean
clean:
	@echo "  CLEAN   $(BUILD_DIR)"
	@$(RM) $(BUILD_DIR)

# Show build configuration
.PHONY: info
info:
	@echo "Mode:           $(MODE)"
	@echo "Platform:       $(PLATFORM)"
	@echo "Compiler:       $(CC)"
	@echo "Assembler:      $(NASM)"
	@echo "Source dir:     $(SRC_DIR)"
	@echo "Build dir:      $(BUILD_DIR)"
	@echo "C Sources:      $(words $(C_SOURCES)) files"
	@echo "ASM Sources:    $(words $(ASM_SOURCES)) files"
	@echo "Output ISO:     $(ISO_FILE)"
	@echo "Kernel ELF:     $(KERNEL_ELF)"
	@echo ""
	@echo "Compiler flags: $(CFLAGS)"
	@echo ""
	@echo "Linker flags:   $(LDFLAGS)"

# Force rebuild
.PHONY: rebuild
rebuild: clean all

# Help target
.PHONY: help
help:
	@echo "Available targets:"
	@echo "  all         - Build ISO and disk image (default)"
	@echo "  run         - Build and run in QEMU"
	@echo "  debug       - Build and run in QEMU with debug server"
	@echo "  clean       - Remove all generated artifacts"
	@echo "  rebuild     - Clean and build"
	@echo "  info        - Show build configuration"
	@echo "  help        - Show this help message"
	@echo ""
	@echo "Build modes (set MODE variable):"
	@echo "  Debug       - Debug build with symbols"
	@echo "  Release     - Optimized release build (default)"

# ==============================================================================
# Dependency Inclusion
# ==============================================================================
-include $(DEPS)
