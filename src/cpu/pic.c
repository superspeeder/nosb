//
// Created by andy on 4/8/26.
//

#include "pic.h"

#include "ports.h"

static inline void io_wait(void) {
    outb(0x80, 0);
}

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC2_COMMAND PIC2
#define PIC1_DATA (PIC1 + 1)
#define PIC2_DATA (PIC2 + 1)
#define PIC_EOI 0x20
#define PIC_READ_IRR 0x0a
#define PIC_READ_ISR 0x0b

#define ICW1_ICW4 0x01
#define ICW1_SINGLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x08
#define ICW4_BUF_MASTER 0x0C
#define ICW4_SFNM 0x10
#define CASCADE_IRQ 2


void pic_remap(int offset1, int offset2) {
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    outb(PIC1_DATA, offset1);
    io_wait();
    outb(PIC2_DATA, offset2);
    io_wait();
    outb(PIC1_DATA, 1 << CASCADE_IRQ);
    io_wait();
    outb(PIC2_DATA, 2);
    io_wait();

    outb(PIC1_DATA, ICW4_8086);
    io_wait();
    outb(PIC2_DATA, ICW4_8086);
    io_wait();

    outb(PIC1_DATA, 0);
    outb(PIC2_DATA, 0);
}

void pic_disable() {
    outb(PIC1_DATA, 0xff);
    outb(PIC2_DATA, 0xff);
}

void pic_init() {
    pic_remap(0x20, 0x28);
}

void pic_irq_set_mask(uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    uint8_t value = inb(port) | (1 << irq_line);
    outb(port, value);
}

void pic_irq_clear_mask(uint8_t irq_line) {
    uint16_t port;

    if (irq_line < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        irq_line -= 8;
    }

    uint8_t value = inb(port) & ~(1 << irq_line);
    outb(port, value);
}

void pic_send_eoi(uint8_t irq) {
    if (irq >= 8) {
        outb(PIC2_COMMAND, PIC_EOI);
    }
    outb(PIC1_COMMAND, PIC_EOI);
}

static uint16_t _pic_get_irq_reg(int ocw3) {
    outb(PIC1_COMMAND, ocw3);
    outb(PIC2_COMMAND, ocw3);
    return (inb(PIC2_COMMAND) << 8) | inb(PIC1_COMMAND);
}

uint16_t pic_get_irr() {
    return _pic_get_irq_reg(PIC_READ_IRR);
}

uint16_t pic_get_isr() {
    return _pic_get_irq_reg(PIC_READ_ISR);
}

