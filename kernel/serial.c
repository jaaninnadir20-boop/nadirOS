#include "serial.h"
#include "io.h"

#define COM1 0x3F8

static bool serial_ready;

bool serial_init(void)
{
    outb(COM1 + 1, 0x00);   /* disable UART interrupts */
    outb(COM1 + 3, 0x80);   /* DLAB on */
    outb(COM1 + 0, 0x01);   /* divisor 1 -> 115200 baud */
    outb(COM1 + 1, 0x00);
    outb(COM1 + 3, 0x03);   /* 8 data bits, no parity, 1 stop bit */
    outb(COM1 + 2, 0xC7);   /* enable and clear FIFOs */
    outb(COM1 + 4, 0x1E);   /* loopback mode for self-test */
    outb(COM1 + 0, 0xAE);

    if (inb(COM1 + 0) != 0xAE) {
        serial_ready = false;
        return false;
    }

    outb(COM1 + 4, 0x0F);   /* normal operation */
    serial_ready = true;
    return true;
}

void serial_putc(char c)
{
    if (!serial_ready)
        return;
    while ((inb(COM1 + 5) & 0x20) == 0)
        ;
    outb(COM1, (uint8_t)c);
}

void serial_puts(const char *s)
{
    for (; *s != '\0'; s++) {
        if (*s == '\n')
            serial_putc('\r');
        serial_putc(*s);
    }
}

void serial_put_hex(uint64_t value)
{
    static const char digits[] = "0123456789abcdef";

    serial_puts("0x");
    for (int shift = 60; shift >= 0; shift -= 4)
        serial_putc(digits[(value >> shift) & 0xF]);
}

void serial_put_dec(uint64_t value)
{
    char buf[21];
    int i = 0;

    if (value == 0) {
        serial_putc('0');
        return;
    }
    while (value != 0) {
        buf[i++] = (char)('0' + value % 10);
        value /= 10;
    }
    while (i > 0)
        serial_putc(buf[--i]);
}