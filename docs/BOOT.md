# First Boot Milestone

The first kernel milestone is intentionally tiny.

## Expected flow

Firmware/bootloader -> x86_64 entry point -> C kernel -> VGA text output.

The kernel currently writes directly to the legacy VGA text buffer at 0xB8000.

This is an early bring-up mechanism only. Later, nadirOS will replace it with a proper framebuffer/graphics console.

## Next step

Add a reproducible bootloader/image build and an emulator test.
