#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "limine/limine.h"
#include "serial.h"

/* ---- Limine boot protocol requests -------------------------------------- */

__attribute__((used, section(".limine_requests")))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(3);

__attribute__((used, section(".limine_requests")))
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST_ID,
    .revision = 0,
};

__attribute__((used, section(".limine_requests_start")))
static volatile uint64_t limine_requests_start[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end")))
static volatile uint64_t limine_requests_end[] = LIMINE_REQUESTS_END_MARKER;

/* ---- Helpers ------------------------------------------------------------ */

static void halt_forever(void)
{
    for (;;)
        __asm__ volatile ("cli; hlt");
}

/* Fill the whole framebuffer with a solid colour (32 bpp only). Returns false
 * if the framebuffer format is not one we understand. */
static bool framebuffer_fill(const struct limine_framebuffer *fb, uint32_t rgb)
{
    if (fb->bpp != 32)
        return false;

    for (uint64_t y = 0; y < fb->height; y++) {
        volatile uint32_t *row = (volatile uint32_t *)((uint8_t *)fb->address + y * fb->pitch);
        for (uint64_t x = 0; x < fb->width; x++)
            row[x] = rgb;
    }
    return true;
}

void kernel_main(void)
{
    bool have_serial = serial_init();

    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        serial_puts("nadirOS: bootloader does not support the requested Limine base revision\n");
        halt_forever();
    }

    (void)have_serial;
    serial_puts("nadirOS kernel initialized.\n");

    if (hhdm_request.response != NULL) {
        serial_puts("hhdm offset: ");
        serial_put_hex(hhdm_request.response->offset);
        serial_puts("\n");
    } else {
        serial_puts("hhdm: no response\n");
    }

    if (framebuffer_request.response != NULL && framebuffer_request.response->framebuffer_count > 0) {
        const struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

        serial_puts("framebuffer: ");
        serial_put_dec(fb->width);
        serial_puts("x");
        serial_put_dec(fb->height);
        serial_puts("x");
        serial_put_dec(fb->bpp);
        serial_puts(" pitch ");
        serial_put_dec(fb->pitch);
        serial_puts("\n");

        if (framebuffer_fill(fb, 0x00203060))
            serial_puts("framebuffer: filled\n");
        else
            serial_puts("framebuffer: unsupported pixel format\n");
    } else {
        serial_puts("framebuffer: none provided\n");
    }

    serial_puts("nadirOS: halting.\n");
    halt_forever();
}
