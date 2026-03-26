CC = x86_64-elf-gcc
LD = x86_64-elf-ld

CFLAGS = -ffreestanding -nostdlib -Wall -Wextra
QEMU_FLAGS = -m 8G

.PHONY: all
all: bin/boot.bin

bin/%.bin: src/%.asm
	mkdir -p $(dir $@)
	nasm $< -f bin -o $@ -I src/

.PHONY: run
run: bin/boot.bin
	qemu-system-x86_64 $(QEMU_FLAGS) -drive file=$<,format=raw,index=0,media=disk
