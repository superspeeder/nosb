CC = x86_64-elf-gcc
LD = x86_64-elf-ld

CFLAGS = -ffreestanding -nostdlib -Wall -Wextra -O2 -Isrc/
LDFLAGS = -ffreestanding -O2 -nostdlib
QEMU_FLAGS = -m 8G

.PHONY: all
all: bin/kernel.elf

bin/%.asm.o: src/%.asm
	mkdir -p $(dir $@)
	nasm $< -f elf32 -o $@ -I src/

bin/%.o: src/%.c
	mkdir -p $(dir $@)
	i686-elf-gcc -c $< -o $@ $(CFLAGS)

bin/kernel.elf: bin/kernel.o bin/boot.asm.o
	mkdir -p $(dir $@)
	i686-elf-gcc -T linker.ld -o $@ $(LDFLAGS) $^ -lgcc

.PHONY: run
run: bin/kernel.elf
	qemu-system-x86_64 $(QEMU_FLAGS) -kernel $<

.PHONY: clean
clean:
	rm -rf bin/