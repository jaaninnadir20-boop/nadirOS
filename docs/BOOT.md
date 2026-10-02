# First Boot Milestone

The first milestone is:

Firmware -> Limine -> x86_64 kernel entry -> C kernel -> VGA text output.

The kernel currently writes directly to the legacy VGA text buffer at 0xB8000.

## Build prerequisites

A Linux/WSL environment needs:

- GCC and binutils
- NASM
- xorriso
- QEMU
- autoconf, automake, mtools for building Limine

## Build the kernel

From the repository root:

    make -C build

This produces:

    build/nadiros.elf

## Build the ISO

First build Limine, then:

    make -C build iso

This produces:

    build/nadiros.iso

## Run

    make -C build run

The first expected kernel message is:

    nadirOS kernel initialized.

This is intentionally a tiny bring-up kernel. Memory management, interrupts, hardware discovery, storage, filesystem, userspace, graphics, and applications come later.
