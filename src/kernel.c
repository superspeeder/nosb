#include "multiboot2.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "cpu/idt.h"

void *s_mbi;
void  outb(uint16_t port, uint8_t value) {
    asm("out %%al, %%dx" ::"a"(value), "d"(port));
}

void outw(uint16_t port, uint16_t value) {
    asm("out %%ax, %%dx" ::"a"(value), "d"(port));
}

void outd(uint16_t port, uint32_t value) {
    asm("out %%eax, %%dx" ::"a"(value), "d"(port));
}

uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile("in %%dx, %%al" : "=a"(value) : "d"(port));
    return value;
}

uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile("in %%dx, %%ax" : "=a"(value) : "d"(port));
    return value;
}

uint32_t ind(uint16_t port) {
    uint32_t value;
    asm volatile("in %%dx, %%eax" : "=a"(value) : "d"(port));
    return value;
}

enum {
    COM0 = 0x3f8,
};

void init_ser() {
    outb(COM0 + 1, 0x00);
    outb(COM0 + 3, 0x80);
    outb(COM0 + 0, 0x03);
    outb(COM0 + 1, 0x00);
    outb(COM0 + 3, 0x03);
    outb(COM0 + 2, 0xC7);
    outb(COM0 + 4, 0x0B);
    outb(COM0 + 4, 0x1E);
    outb(COM0 + 0, 0xAE);
    inb(COM0 + 0);
    outb(COM0 + 4, 0x0F);
}

int ser_recv() {
    return inb(COM0 + 5) & 1;
}

char read_ser() {
    while (ser_recv() == 0)
        ;
    return inb(COM0);
}

int is_transmit_empty() {
    return inb(COM0 + 5) & 0x20;
}

void write_ser(char a) {
    while (is_transmit_empty() == 0)
        ;
    outb(COM0, a);
}

void print_ser(const char *s) {
    while (*s) {
        write_ser(*s);
        ++s;
    }
}

void print_uint(unsigned int n) {
    unsigned int dig = n % 10;
    unsigned int rem = n / 10;
    if (rem > 0) {
        print_uint(rem);
    }

    write_ser('0' + dig);
}

void print_int(int n) {
    if (n < 0) {
        write_ser('-');
        print_uint((unsigned int)-n);
    } else {
        print_uint((unsigned int)n);
    }
}

__attribute__((noreturn)) void kernel_main(void *mbi) {
    s_mbi                                = mbi;
    struct multiboot_tag_framebuffer *fb = NULL;

    init_ser();
    print_ser("Hello\r\n");
    idt_init();

    asm volatile ("int $8" :: );


    // uint32_t mbi_size = *(uint32_t *)mbi;
    // void    *e        = (unsigned char *)mbi + mbi_size;
    // mbi               = (unsigned char *)mbi + 8;
    // while (mbi < e) {
    //     struct multiboot_tag *tag = mbi;
    //     print_ser("MBI Tag: ");
    //     print_uint(tag->type);
    //     print_ser("\r\n");
    //     if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
    //         fb = mbi;
    //     }

    //     mbi = (unsigned char *)mbi + ((tag->size + 7) & ~7);
    // }

    // if (fb) {
    //     print_ser("Framebuffer!\r\n");
    //     uint32_t *fba = (uint32_t *)fb->common.framebuffer_addr;
    //     // print_uint((uint32_t)((uint64_t)fba >> 32));
    //     print_uint((uint32_t)((uint64_t)fba));
    //     for (uint32_t y = 0; y < fb->common.framebuffer_height; ++y) {
    //         for (uint32_t x = 0; x < fb->common.framebuffer_width; ++x) {
    //             fba[y * fb->common.framebuffer_pitch / 4 + x] = 0x00fffff;
    //         }
    //     }
    // }

    asm volatile("cli");
    while (true) {
        asm volatile("hlt");
    }
}
