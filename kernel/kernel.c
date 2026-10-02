#include <stdint.h>

static volatile uint16_t *const VGA = (uint16_t *)0xB8000;

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
    terminal_write("nadirOS kernel initialized.");
}
