# Build configuration
MODE ?= Release
PLATFORM := amd64

# ==============================================================================
# Tool Configuration
# ==============================================================================
CC := clang
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

# Limine bootloader
LIMINE_DIR := stand/limine
LIMINE_CFG := $(LIMINE_DIR)/limine.conf

# ==============================================================================
# Output Files
# ==============================================================================
KERNEL_ELF := $(OUT_DIR)/wt.elf
KERNEL_BOOT := $(BOOT_DIR)/wt.elf
ISO_FILE := $(OUT_DIR)/wt.iso
DISK_FILE := $(BUILD_DIR)/hard-drive.img

# ==============================================================================
# Limine Files
# ==============================================================================
LIMINE_FILES := \
	$(LIMINE_DIR)/limine-bios.sys \
	$(LIMINE_DIR)/limine-bios-cd.bin \
	$(LIMINE_DIR)/limine-uefi-cd.bin \
	$(LIMINE_DIR)/BOOTX64.EFI

# ==============================================================================
# Compiler Flags
# ==============================================================================
# Base C flags (common to all modes)
# Base C flags (common to all modes)
BASE_CFLAGS := \
	-std=c17 \
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
    MODE_CFLAGS := -O3 -march=x86-64 -mtune=generic -fno-omit-frame-pointer
    STRIP_DEBUG := true
else
    $(error Invalid MODE '$(MODE)'. Use 'Debug' or 'Release')
endif

# Combined C flags
CFLAGS := $(BASE_CFLAGS) $(WARNING_CFLAGS) $(OPTIMIZE_CFLAGS) $(MODE_CFLAGS)

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

# ==============================================================================
# Source Files and Objects
# ==============================================================================
C_SOURCES := $(shell (find $(SRC_DIR) -type f -name '*.c' | grep -v '^$(SRC_DIR)/arch/' ; find $(SRC_DIR)/arch/$(PLATFORM) -type f -name '*.c' 2>/dev/null) )
ASM_SOURCES := $(shell (find $(SRC_DIR) -type f -name '*.asm' | grep -v '^$(SRC_DIR)/arch/' ; find $(SRC_DIR)/arch/$(PLATFORM) -type f -name '*.asm' 2>/dev/null) )

C_OBJECTS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(C_SOURCES))
ASM_OBJECTS := $(patsubst $(SRC_DIR)/%.asm,$(OBJ_DIR)/%.o,$(ASM_SOURCES))
ALL_OBJECTS := $(C_OBJECTS) $(ASM_OBJECTS)

# Dependency files
DEPS := $(C_OBJECTS:.o=.d)

# Linker script
LINKER_SCRIPT := $(SRC_DIR)/arch/$(PLATFORM)/linker.ld

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
	-machine hpet=on \
	-m 128M \
	-serial mon:stdio \
	-drive file=$(DISK_FILE),format=raw,if=ide

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
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
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