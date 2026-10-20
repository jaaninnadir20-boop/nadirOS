# nadirOS

A real, bootable operating-system project built from the ground up.

## Project goals

- Boot on x86_64 UEFI/BIOS-compatible environments
- Develop a small kernel with a clear hardware abstraction boundary
- Provide memory, interrupts, input, storage, and process foundations
- Build a graphical desktop and application layer later
- Keep the project modular, documented, and testable

## Development roadmap

1. Bootloader and kernel entry
2. Kernel console and CPU setup
3. Interrupts and memory management
4. Keyboard and basic storage
5. Filesystem and userspace
6. Shell and system utilities
7. Graphics stack and desktop
8. Applications
9. Installer and reproducible ISO builds

The first milestone is intentionally small: produce a bootable kernel image and verify it in an emulator before targeting physical hardware.

## Repository layout

- `boot/` — boot entry and bootloader configuration
- `kernel/` — kernel source
- `drivers/` — hardware drivers
- `filesystem/` — filesystem code and definitions
- `apps/` — userspace applications
- `build/` — build scripts and generated artifacts
- `docs/` — architecture and development documentation
- `config/` — boot and project configuration

## CI

Every push and pull request builds the kernel and packages a bootable ISO.
