# Build

`Makefile` is the single build entry point; see `docs/BOOT.md` for prerequisites and commands.

    make -C build          # kernel
    make -C build test     # kernel + disk image + QEMU boot test
    make -C build clean

Generated files (`nadiros.elf`, `nadiros.img`, `nadiros.iso`, `out/`, `limine/`) are git-ignored.
