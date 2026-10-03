#include <stdint.h>
#include "limine.h"

/*
 * Limine boot protocol metadata.
 * Base revision 6 is supported by Limine 12.9.1.
 */
__attribute__((used, section(".limine_requests"), aligned(8)))
static volatile uint64_t limine_base_revision[] = LIMINE_BASE_REVISION(6);

__attribute__((used, section(".limine_requests_start"), aligned(8)))
static volatile uint64_t limine_requests_start_marker[] = LIMINE_REQUESTS_START_MARKER;

__attribute__((used, section(".limine_requests_end"), aligned(8)))
static volatile uint64_t limine_requests_end_marker[] = LIMINE_REQUESTS_END_MARKER;

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;

static void debug_write(const char *text)
{
    while (*text != '\0') {
        __asm__ volatile ("outb %0, %1" : : "a"(*text), "Nd"((uint16_t)0xE9));
        text++;
    }
}

static void terminal_write(const char *text)
{
    uint16_t i = 0;

    while (text[i] != '\0') {
        VGA[i] = (uint16_t)text[i] | ((uint16_t)0x07 << 8);
        i++;
    }
}

void kernel_main(void)
{
    if (!LIMINE_BASE_REVISION_SUPPORTED(limine_base_revision)) {
        for (;;) {
            __asm__ volatile ("cli; hlt");
        }
    }

    const char *message = "nadirOS kernel initialized.";
    terminal_write(message);
    debug_write(message);

    for (;;) {
        __asm__ volatile ("hlt");
    }
}
