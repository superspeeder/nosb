#include "multiboot2.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "cpu/idt.h"
#include "cpu/pic.h"
#include "cpu/ports.h"
#include "drivers/serial.h"
#include "klib/klog.h"

void *s_mbi;

extern bool paging_5;

__attribute__((noreturn)) void kernel_main(void *mbi) {
    s_mbi                                = mbi;
    struct multiboot_tag_framebuffer *fb = NULL;

    serial_init();
    klog_info("Hello!");
    if (paging_5) {
        klog_info("5-level paging");
    }

    idt_init();
    pic_irq_set_mask(0);

    uint32_t mbi_size = *(uint32_t *)mbi;
    void    *e        = (unsigned char *)mbi + mbi_size;
    mbi               = (unsigned char *)mbi + 8;
    while (mbi < e) {
        struct multiboot_tag *tag = mbi;
        klog_begin_complex();
        klog_info("MBI Tag: ");
        klog_complex_int_dec(tag->type);
        klog_newline();
        if (tag->type == MULTIBOOT_TAG_TYPE_FRAMEBUFFER) {
            fb = mbi;
        }

        mbi = (unsigned char *)mbi + ((tag->size + 7) & ~7);
    }

    if (fb) {
        uint32_t *fba = (uint32_t *)fb->common.framebuffer_addr;
        for (uint32_t y = 0; y < fb->common.framebuffer_height; ++y) {
            for (uint32_t x = 0; x < fb->common.framebuffer_width; ++x) {
                fba[y * fb->common.framebuffer_pitch / 4 + x] = 0x00fffff;
            }
        }
    }

    // asm volatile("cli");
    while (true) {
        asm volatile("hlt");
        klog_info("Resume");
    }
}
