TARGET := nadiros
CC ?= gcc
LD ?= ld
AS := nasm
LIMINE_DIR ?= limine
QEMU ?= qemu-system-x86_64

CFLAGS := -std=c11 -ffreestanding -fno-pie -fno-stack-protector -mno-red-zone -mno-mmx -mno-sse -mno-sse2 -mcmodel=small -Wall -Wextra -O2
LDFLAGS := -nostdlib -static -z max-page-size=0x1000

ROOT := ..
KERNEL := $(ROOT)/build/$(TARGET).elf
ISO_ROOT := $(ROOT)/build/iso_root
ISO := $(ROOT)/build/$(TARGET).iso

all: $(KERNEL)

entry.o: $(ROOT)/kernel/entry.asm
	$(AS) -f elf64 $< -o $@

kernel.o: $(ROOT)/kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(KERNEL): entry.o kernel.o $(ROOT)/kernel/linker.ld
	$(LD) $(LDFLAGS) -T $(ROOT)/kernel/linker.ld entry.o kernel.o -o $@

iso: $(KERNEL)
	rm -rf $(ISO_ROOT)
	mkdir -p $(ISO_ROOT)/boot
	cp $(KERNEL) $(ISO_ROOT)/boot/nadiros.elf
	cp $(ROOT)/boot/limine.conf $(ISO_ROOT)/limine.conf
	cp $(LIMINE_DIR)/bin/limine-bios-cd.bin $(ISO_ROOT)/boot/limine-bios-cd.bin
	cp $(LIMINE_DIR)/bin/limine-uefi-cd.bin $(ISO_ROOT)/boot/limine-uefi-cd.bin
	xorriso -as mkisofs \
		-b boot/limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot boot/limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		$(ISO_ROOT) -o $(ISO)
	$(LIMINE_DIR)/bin/limine bios-install $(ISO)

run: iso
	$(QEMU) -cdrom $(ISO)

clean:
	rm -f entry.o kernel.o $(ROOT)/build/*.elf $(ROOT)/build/*.iso
	rm -rf $(ISO_ROOT)

.PHONY: all iso run clean
