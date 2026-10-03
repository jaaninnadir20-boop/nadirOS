# nadirOS build system
#
# Entry point (from a clean checkout, any working directory):
#     make -C build            # build build/nadiros.elf and verify its format
#     make -C build iso        # build build/nadiros.iso (needs Limine + xorriso)
#     make -C build run        # boot the ISO in QEMU
#     make -C build clean
#
# Generated files: build/nadiros.elf, build/nadiros.iso, build/out/ (all git-ignored).

TARGET := nadiros
CC     ?= gcc
LD     ?= ld
AS     := nasm
QEMU   ?= qemu-system-x86_64

# Absolute paths: independent of the caller's working directory.
BUILD_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
ROOT      := $(abspath $(BUILD_DIR)/..)
OUT       := $(BUILD_DIR)/out
OBJ       := $(OUT)/obj
LIMINE_DIR ?= $(BUILD_DIR)/limine

KERNEL   := $(BUILD_DIR)/$(TARGET).elf
ISO_ROOT := $(OUT)/iso_root
ISO      := $(BUILD_DIR)/$(TARGET).iso

CFLAGS  := -std=c11 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone \
           -mno-mmx -mno-sse -mno-sse2 -mcmodel=kernel -Wall -Wextra -Werror -O2 \
           -I$(ROOT)/kernel/include -MMD -MP
ASFLAGS := -f elf64 -Werror
LDFLAGS := -nostdlib -static -z max-page-size=0x1000 -z noexecstack

OBJS := $(OBJ)/entry.o $(OBJ)/kernel.o $(OBJ)/serial.o
DEPS := $(OBJS:.o=.d)

all: $(KERNEL) verify

# Fail early, with a clear message, if a required tool is missing.
check-tools:
	@for t in $(CC) $(LD) $(AS) readelf; do \
	    command -v $$t >/dev/null 2>&1 || { echo "error: required tool '$$t' not found in PATH" >&2; exit 1; }; \
	done

$(OBJ):
	mkdir -p $@

$(OBJ)/entry.o: $(ROOT)/kernel/entry.asm | check-tools $(OBJ)
	$(AS) $(ASFLAGS) $< -o $@

$(OBJ)/%.o: $(ROOT)/kernel/%.c | check-tools $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): $(OBJS) $(ROOT)/kernel/linker.ld
	$(LD) $(LDFLAGS) -T $(ROOT)/kernel/linker.ld $(OBJS) -o $@

# Structural check of the produced kernel: ELF64, x86-64, entry in the higher half.
verify: $(KERNEL)
	@readelf -h $(KERNEL) | grep -q 'Class:.*ELF64'        || { echo "error: $(KERNEL) is not ELF64" >&2; exit 1; }
	@readelf -h $(KERNEL) | grep -q 'Machine:.*X86-64'     || { echo "error: $(KERNEL) is not x86-64" >&2; exit 1; }
	@readelf -h $(KERNEL) | grep -q 'Entry point address:.*0xffffffff80' || { echo "error: entry point not in higher half" >&2; exit 1; }
	@echo "verify: $(KERNEL) is a valid ELF64 x86-64 kernel"

# Limine file layout: a source build puts files in $(LIMINE_DIR)/bin, a *-binary
# branch checkout puts them in $(LIMINE_DIR). Both are supported.
LIMINE_BIN = $(firstword $(wildcard $(LIMINE_DIR)/bin) $(LIMINE_DIR))

iso: $(KERNEL) verify
	@for t in xorriso; do command -v $$t >/dev/null 2>&1 || { echo "error: required tool '$$t' not found in PATH" >&2; exit 1; }; done
	@test -f $(LIMINE_BIN)/limine-bios-cd.bin || { echo "error: Limine not found in $(LIMINE_DIR) (see docs/BOOT.md)" >&2; exit 1; }
	rm -rf $(ISO_ROOT)
	mkdir -p $(ISO_ROOT)/boot/limine $(ISO_ROOT)/EFI/BOOT
	cp $(KERNEL) $(ISO_ROOT)/boot/nadiros.elf
	cp $(ROOT)/boot/limine.conf $(ISO_ROOT)/boot/limine/limine.conf
	cp $(LIMINE_BIN)/limine-bios.sys $(LIMINE_BIN)/limine-bios-cd.bin $(LIMINE_BIN)/limine-uefi-cd.bin $(ISO_ROOT)/boot/limine/
	cp $(LIMINE_BIN)/BOOTX64.EFI $(ISO_ROOT)/EFI/BOOT/
	xorriso -as mkisofs -R -r -J \
	    -b boot/limine/limine-bios-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
	    -hfsplus -apm-block-size 2048 \
	    --efi-boot boot/limine/limine-uefi-cd.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
	    $(ISO_ROOT) -o $(ISO)
	$(LIMINE_DIR)/limine bios-install $(ISO) || $(LIMINE_BIN)/limine bios-install $(ISO)

# BIOS-bootable raw disk image (no xorriso needed); used for emulator boot tests.
DISK := $(BUILD_DIR)/$(TARGET).img
disk: $(KERNEL) verify
	$(ROOT)/scripts/mkdisk.sh $(KERNEL) $(ROOT)/boot/limine.conf $(LIMINE_BIN) $(DISK)

# Boot the disk image in QEMU and check the serial log (exit 77 = QEMU not installed).
test: disk
	QEMU="$(QEMU)" $(ROOT)/scripts/boot-test.sh $(DISK)

run: iso
	$(QEMU) -cdrom $(ISO) -serial stdio

clean:
	rm -rf $(OUT) $(KERNEL) $(ISO) $(DISK)

-include $(DEPS)

.PHONY: all check-tools verify iso disk test run clean
