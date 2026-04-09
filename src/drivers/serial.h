//
// Created by andy on 4/8/26.
//

#pragma once
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t port;
    bool     initialized;
} serial_port_t;

void serial_init(void);
void serial_init_port(serial_port_t *port);

void serial_disable_interrupts(serial_port_t *port);
void serial_set_baud(serial_port_t *port, int baud);
void serial_set_8n1(serial_port_t *port);
void serial_set_fifo(serial_port_t *port);
void serial_enable_irqs_rts_dsr(serial_port_t *port);
void serial_test_port(serial_port_t *port);
void serial_set_normal(serial_port_t *port);

bool serial_is_received(serial_port_t *port);
bool serial_is_transmitted(serial_port_t *port);

char serial_getchar(serial_port_t *port);
void serial_putchar(serial_port_t *port, char c);

void serial_print(serial_port_t *port, const char *s);

extern serial_port_t* default_serial_port;