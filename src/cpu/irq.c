//
// Created by andy on 4/8/26.
//

#include "klib/klog.h"
#include "pic.h"
#include "ports.h"


#include <stdint.h>

void irq_handler(uint8_t irq_number) {
    pic_send_eoi(irq_number);
    if (irq_number == 0) return;
    if (irq_number == 1) {
        inb(0x60);
    }

    klog_begin_complex();
    klog_info("IRQ ");
    klog_complex_uint_dec(irq_number);
    klog_newline();
}