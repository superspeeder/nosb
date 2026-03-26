#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

__attribute__((noreturn)) void kernel_main() {
    for (size_t y = 0 ; y < VGA_HEIGHT ; y++) {
        for (size_t x = 0 ; x < VGA_WIDTH ; x++) {
            terminal_buffer[y * VGA_WIDTH + x] = ((uint16_t)' ') | 0xf000;
        }
    }

    asm volatile ("cli");
    while (true) {
        asm volatile("hlt");
    }
}
