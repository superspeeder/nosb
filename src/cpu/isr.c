#include "isr.h"

#include <stdint.h>

void print_ser(const char *s);
void print_uint(unsigned int n);

__attribute__((noreturn)) void exception_handler(uint8_t interrupt_number) {
    print_ser("Exception!\r\n");
    print_uint(interrupt_number);
    print_ser("\r\n");
    
    while (1) {
        asm volatile("cli; hlt");
    }
}
