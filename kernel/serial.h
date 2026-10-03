#ifndef NADIROS_SERIAL_H
#define NADIROS_SERIAL_H

#include <stdbool.h>
#include <stdint.h>

/* Initialise COM1 (115200 8N1). Returns false if no working UART was found. */
bool serial_init(void);
void serial_putc(char c);
void serial_puts(const char *s);
void serial_put_hex(uint64_t value);
void serial_put_dec(uint64_t value);

#endif