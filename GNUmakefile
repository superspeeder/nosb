CC = x86_64-elf-gcc
LD = x86_64-elf-ld

CFLAGS = -ffreestanding -nostdlib -Wall -Wextra -O2 -Isrc/
LDFLAGS = -ffreestanding -O2 -nostdlib
QEMU_FLAGS = -m 8G -serial stdio

.PHONY: all
all: bin/os.iso

bin/%.nasm.o: src/%.nasm
	mkdir -p $(dir $@)
	nasm $< -f elf64 -o $@ -I src/

bin/%.o: src/%.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CFLAGS)

bin/kernel.elf: bin/kernel.o bin/boot.nasm.o
	mkdir -p $(dir $@)
	$(CC) -T linker.ld -o $@ $(LDFLAGS) $^ -lgcc

bin/os.iso: bin/kernel.elf grub.cfg
	mkdir -p isodir/boot/grub
	cp bin/kernel.elf isodir/boot/kernel.elf
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o $@ isodir
	rm -r isodir

.PHONY: run
run: bin/os.iso
	qemu-system-x86_64 $(QEMU_FLAGS) -drive file=$<,index=2,media=cdrom

.PHONY: clean
clean:
	rm -rf bin/

.PHONY: run-debug
run-debug: bin/os.iso
	qemu-system-x86_64 -cdrom bin/os.iso -no-reboot -d int,cpu_reset