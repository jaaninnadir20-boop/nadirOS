TARGET := nadiros
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
AS := nasm

CFLAGS := -ffreestanding -mno-red-zone -mno-sse -mno-sse2 -Wall -Wextra -O2
LDFLAGS := -nostdlib -z max-page-size=0x1000

all: $(TARGET).elf

kernel/entry.o: kernel/entry.asm
	$(AS) -f elf64 $< -o $@

kernel/kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: kernel/entry.o kernel/kernel.o kernel/linker.ld
	$(LD) $(LDFLAGS) -T kernel/linker.ld kernel/entry.o kernel/kernel.o -o $@

clean:
	rm -f kernel/*.o $(TARGET).elf
