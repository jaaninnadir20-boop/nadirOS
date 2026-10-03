#!/bin/sh
# Boot the disk image in QEMU and check the kernel's serial output.
# Exit status: 0 = PASS, 1 = FAIL, 77 = SKIPPED (QEMU or firmware files unavailable).
#
# usage: boot-test.sh <disk.img>
# env:   QEMU      emulator binary (default qemu-system-x86_64)
#        QEMU_ARGS extra arguments (e.g. "-L /path/to/pc-bios")
#        TIMEOUT   seconds to wait for the kernel (default 30)
set -u

DISK=$1
QEMU=${QEMU:-qemu-system-x86_64}
TIMEOUT=${TIMEOUT:-30}
QEMU_ARGS=${QEMU_ARGS:-}

command -v "$QEMU" >/dev/null 2>&1 || { echo "boot-test: SKIPPED ($QEMU not found)"; exit 77; }
[ -f "$DISK" ] || { echo "boot-test: FAIL (disk image not found: $DISK)"; exit 1; }

LOG=$(mktemp); trap 'kill $PID 2>/dev/null; rm -f "$LOG"' EXIT

# shellcheck disable=SC2086
"$QEMU" $QEMU_ARGS -machine q35 -m 256 -drive file="$DISK",format=raw \
    -display none -serial file:"$LOG" -no-reboot >/dev/null 2>&1 &
PID=$!

i=0
while [ "$i" -lt $((TIMEOUT * 2)) ]; do
    grep -q 'nadirOS: halting\.' "$LOG" 2>/dev/null && break
    kill -0 "$PID" 2>/dev/null || break   # QEMU exited early (e.g. triple fault with -no-reboot)
    sleep 0.5; i=$((i + 1))
done
kill "$PID" 2>/dev/null; wait "$PID" 2>/dev/null

status=0
for pat in 'nadirOS kernel initialized\.' 'hhdm offset: 0x' 'framebuffer: [0-9]*x[0-9]*x32' 'framebuffer: filled' 'nadirOS: halting\.'; do
    if grep -q "$pat" "$LOG"; then echo "  ok:      $pat"; else echo "  MISSING: $pat"; status=1; fi
done

if [ "$status" -eq 0 ]; then echo "boot-test: PASS"; else echo "boot-test: FAIL"; echo "--- serial log:"; cat -v "$LOG"; fi
exit $status