#include <stdint.h>

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
    const char *message = "nadirOS kernel initialized.";
    terminal_write(message);
    debug_write(message);
}
