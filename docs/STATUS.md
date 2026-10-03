# nadirOS Status

Last verified: 2026-10-03. Evidence = commands in `docs/BOOT.md`; "NOT RUN" means no test was possible in the verification environment.

## Milestones

| ID | Milestone | Status | Evidence |
|---|---|---|---|
| M1 | Reproducible build | VERIFIED | Clean build from a fresh copy of the tree: `make -C build` exits 0; ELF64 x86-64, entry `0xffffffff80000000`. |
| M2 | Valid boot artifact | VERIFIED (BIOS disk image only) | `build/nadiros.img`: MBR + FAT32 + Limine; boots. ISO and UEFI: NOT RUN (no xorriso / OVMF). |
| M3 | Verified boot | VERIFIED (QEMU 10.1.0, q35, BIOS) | `make -C build test` prints PASS; serial shows kernel banner. Real hardware and UEFI: NOT RUN. |
| M4 | Functional kernel | IN PROGRESS | Serial + framebuffer fill only. No IDT, PMM, VMM, timer. |
| M5 | Usable system interface | NOT STARTED | |
| M6 | Functional applications | NOT STARTED | |
| M7 | Release candidate | NOT STARTED | |

## Component inventory

| Component | Status | Known defects / limits | Verification |
|---|---|---|---|
| `build/Makefile` | Working | ISO target untested | clean build, `-Werror`, ELF check |
| `scripts/mkdisk.sh` | Working | BIOS only; needs mtools | boots in QEMU |
| `scripts/boot-test.sh` | Working | checks serial strings only | PASS on good image; FAIL on known-bad image |
| `kernel/entry.asm` | Working | fixed 16 KiB stack, no guard page | boots |
| `kernel/serial.c` (COM1) | Working | polling, output only | observed in QEMU |
| `kernel/kernel.c` | Bring-up | solid-colour fill only, no text on screen | serial output; fill not visually inspected |
| Interrupts / memory / drivers / fs / userspace / UI / apps | Not implemented | directories contain only README.md | n/a |

## Defects fixed

1. **Build failed** (`nasm: unable to open output file kernel/entry.o`): Makefile wrote objects to `build/kernel/`, which did not exist. Fix: objects go to `build/out/obj/` (created by the Makefile).
2. **Kernel output could not be seen**: it wrote to VGA text memory (0xB8000), which Limine's graphical handoff does not display. Fix: COM1 serial output + Limine framebuffer/HHDM requests.
3. **ISO recipe omitted `limine-bios.sys`**: BIOS boot of the ISO would fail with "Stage 3 file not found". Fix applied in the Makefile; ISO still NOT RUN.
4. **Missing `.note.GNU-stack`** in `entry.asm`: added.

## Next

P2: IDT + exception handlers with serial panic output, then memory map parsing and a physical memory manager.