#!/bin/sh
# Build a BIOS-bootable raw disk image (MBR + one FAT32 partition + Limine) without
# xorriso, mounting, or root. Used for emulator boot tests.
#
# Limine >= 10 can only read FAT and ISO9660 in its first stage, so the partition
# is FAT32 (created with mtools).
#
# usage: mkdisk.sh <kernel.elf> <limine.conf> <limine-dir> <out.img>
set -eu

KERNEL=$1; CONF=$2; LIMINE=$3; OUT=$4

die() { echo "mkdisk: error: $*" >&2; exit 1; }
for t in mformat mcopy mmd python3; do command -v "$t" >/dev/null 2>&1 || die "required tool '$t' not found (apt install mtools)"; done
[ -f "$KERNEL" ] || die "kernel not found: $KERNEL"
[ -f "$CONF" ] || die "config not found: $CONF"
[ -f "$LIMINE/limine-bios.sys" ] || die "limine-bios.sys not found in $LIMINE"
[ -x "$LIMINE/limine" ] || die "limine tool not built in $LIMINE (run 'make' there)"

SIZE_MIB=64
PART_START_SECTORS=2048
PART_SECTORS=$((SIZE_MIB * 2048 - PART_START_SECTORS))
PART_OFFSET=$((PART_START_SECTORS * 512))

rm -f "$OUT"
dd if=/dev/zero of="$OUT" bs=1M count=$SIZE_MIB status=none

# MBR: one bootable FAT32 (type 0x0C, LBA) partition from LBA 2048 to the end of the disk.
python3 - "$OUT" "$PART_START_SECTORS" "$PART_SECTORS" <<'PY'
import struct, sys
path, first, count = sys.argv[1], int(sys.argv[2]), int(sys.argv[3])
entry = struct.pack("<B3sB3sII", 0x80, b"\x00\x21\x00", 0x0C, b"\xff\xff\xff", first, count)
with open(path, "r+b") as f:
    f.seek(446); f.write(entry)
    f.seek(510); f.write(b"\x55\xaa")
PY

IMG="$OUT@@$PART_OFFSET"
mformat -i "$IMG" -F -c 1 -h 255 -s 63 -T "$PART_SECTORS" -v NADIROS ::
mmd -i "$IMG" ::/boot ::/boot/limine
mcopy -i "$IMG" "$KERNEL" ::/boot/nadiros.elf
mcopy -i "$IMG" "$CONF" ::/boot/limine/limine.conf
mcopy -i "$IMG" "$LIMINE/limine-bios.sys" ::/boot/limine/limine-bios.sys

"$LIMINE/limine" bios-install "$OUT" >/dev/null 2>&1 || die "limine bios-install failed"
echo "mkdisk: wrote $OUT"