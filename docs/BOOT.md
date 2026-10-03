# Boot and Build

Boot path: BIOS -> Limine (BIOS stage 1/2/3) -> Limine protocol -> x86_64 kernel (`kmain`) -> `kernel_main`.

The kernel reports progress on **COM1 (serial)**. Limine's own diagnostics are mirrored to
COM1 as well (`serial: yes` in `boot/limine.conf`). The legacy VGA text buffer is **not**
used: Limine hands over a graphical framebuffer, and UEFI has no VGA text mode.

## Supported / verified configuration

| Item | Version used for verification |
|---|---|
| Host | Ubuntu 24.04, x86_64 |
| GCC / binutils / NASM | 13.3.0 / 2.42 / 2.16.01 |
| Bootloader | Limine v11.x-binary (commit 5be26a7) |
| Emulator | QEMU 10.1.0, `-machine q35`, BIOS (SeaBIOS) |

UEFI boot and the ISO target are **not verified** (no OVMF / xorriso in the verification environment).

## Prerequisites

    sudo apt install build-essential nasm binutils mtools qemu-system-x86 xorriso

Limine (not vendored):

    git clone --depth 1 --branch v11.x-binary https://github.com/Limine-Bootloader/Limine.git build/limine
    make -C build/limine        # builds the `limine` installer tool

## Build

    make -C build               # builds and structurally verifies build/nadiros.elf
    make -C build clean

All intermediates go to `build/out/`; artifacts are `build/nadiros.elf`, `build/nadiros.img`, `build/nadiros.iso` (all git-ignored).
The build stops on the first failing stage, on missing tools, and on any compiler/assembler warning (`-Werror`).

## Boot test (BIOS disk image, no xorriso needed)

    make -C build test          # builds build/nadiros.img, boots it in QEMU, checks serial output

Exit codes of `scripts/boot-test.sh`: 0 pass, 1 fail, 77 QEMU unavailable (skipped).
Override with `QEMU=... QEMU_ARGS="-L <pc-bios dir>"` and `LIMINE_DIR=<dir>`.

Expected serial output:

    nadirOS kernel initialized.
    hhdm offset: 0xffff800000000000
    framebuffer: <w>x<h>x32 pitch <p>
    framebuffer: filled
    nadirOS: halting.

## ISO (unverified)

    make -C build iso

## Notes

* The disk image uses FAT32 because Limine >= 10 can only read FAT and ISO9660 in its first stage
  (an ext2 partition fails with "Stage 3 file not found").
* This is a bring-up kernel: no interrupts/IDT, memory management, or drivers yet. See `docs/STATUS.md`.
