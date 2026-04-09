//
// Created by andy on 4/8/26.
//

#include "serial.h"

#include "cpu/ports.h"
#include "klib/kmem.h"

enum StandardComPort {
    COM1 = 0x3f8,
    COM2 = 0x2f8,
    COM3 = 0x3e8,
    COM4 = 0x2e8,
    COM5 = 0x5f8,
    COM6 = 0x4f8,
    COM7 = 0x5e8,
    COM8 = 0x4e8,
};

static serial_port_t serial_ports[8];
serial_port_t* default_serial_port;

void serial_init(void) {
    memset(serial_ports, 0, sizeof(serial_ports));
    serial_ports[0].port = COM1;
    serial_ports[1].port = COM2;
    serial_ports[2].port = COM3;
    serial_ports[3].port = COM4;
    serial_ports[4].port = COM5;
    serial_ports[5].port = COM6;
    serial_ports[6].port = COM7;
    serial_ports[7].port = COM8;

    serial_init_port(&serial_ports[0]);
    default_serial_port = &serial_ports[0];
}

void serial_init_port(serial_port_t *port) {
    serial_disable_interrupts(port);
    serial_set_baud(port, 38400);
    serial_set_8n1(port);
    serial_set_fifo(port);
    serial_enable_irqs_rts_dsr(port);
    serial_test_port(port);
    serial_set_normal(port);
}

void serial_disable_interrupts(serial_port_t *port) {
    outb(port->port + 1, 0x00);
}

void serial_set_baud(serial_port_t *port, int baud) {
    uint16_t divisor = 115200 / baud;
    outb(port->port + 3, 0x80);
    outb(port->port + 0, divisor & 0xFF);
    outb(port->port + 1, (divisor >> 8) & 0xFF);
}

void serial_set_8n1(serial_port_t *port) {
    outb(port->port + 3, 0x03);
}

void serial_set_fifo(serial_port_t *port) {
    outb(port->port + 2, 0xC7);
}

void serial_enable_irqs_rts_dsr(serial_port_t *port) {
    outb(port->port + 4, 0x0B);
}

void serial_test_port(serial_port_t *port) {
    outb(port->port + 4, 0x1E);
    outb(port->port + 0, 0xAE);
    inb(port->port + 0);
}

void serial_set_normal(serial_port_t *port) {
    outb(port->port + 4, 0x0F);
}

bool serial_is_received(serial_port_t *port) {
    return inb(port->port + 5) & 1;
}

bool serial_is_transmitted(serial_port_t *port) {
    return inb(port->port + 5) & 0x20;
}

char serial_getchar(serial_port_t *port) {
    while (serial_is_received(port) == 0) {}
    return (char)inb(port->port + 0);
}

void serial_putchar(serial_port_t *port, char c) {
    while (serial_is_transmitted(port) == 0) {}
    outb(port->port + 0, c);
}

void serial_print(serial_port_t *port, const char *s) {
    while (*s) {
        serial_putchar(port, *s++);
    }
}
