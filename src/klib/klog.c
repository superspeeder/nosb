//
// Created by andy on 4/8/26.
//

#include "klog.h"

#include "drivers/serial.h"

bool _complex_mode = false;

unsigned _num_padding = 0;
char _numpadchar = '0';

static void _klog_msg(const char* tag, const char* msg) {
    serial_print(default_serial_port, "[");
    serial_print(default_serial_port, tag);
    serial_print(default_serial_port, "] ");
    serial_print(default_serial_port, msg);
    if (!_complex_mode) serial_print(default_serial_port, "\r\n");
}

void klog_info(const char *msg) {
    _klog_msg("\033[32minfo\033[0m", msg);
}

void klog_error(const char *msg) {
    _klog_msg("\033[31merror\033[0m", msg);
}

void klog_warning(const char *msg) {
    _klog_msg("\033[33mwarning\033[0m", msg);
}

void klog_debug(const char *msg) {
    _klog_msg("\033[36mdebug\033[0m", msg);
}

void klog_newline() {
    serial_print(default_serial_port, "\r\n");
    _complex_mode = false;
    klog_reset_fmt_state();
}

void klog_begin_complex() {
    _complex_mode = true;
}

void klog_complex_print(const char *msg) {
    serial_print(default_serial_port, msg);
}

void klog_reset_fmt_state() {
    _num_padding = 0;
    _numpadchar = '0';
}

void klog_set_num_pad(unsigned width) {
    _num_padding = width;
}

static unsigned long long _ipow(unsigned long long b, unsigned x) {
    for (;x--;) {
        b *= b;
    }
    return b;
}

static unsigned _iorder(unsigned long long n, unsigned radix) {
    unsigned order = 1;
    for (;n>radix;n/=radix) {
        ++order;
    }
    return order;
}

static char _radix[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

static void _klog_complex_uint_radix_nopad(unsigned long long num, unsigned radix) {
    if (num > radix) {
        _klog_complex_uint_radix_nopad(num / radix, radix);
    }

    serial_putchar(default_serial_port, _radix[num  % radix]);
}

static void _klog_complex_uint_radix_pad(unsigned long long num, unsigned radix, unsigned zpw) {
    unsigned order_ = _iorder(num, radix);
    if (order_ < zpw) {
        for (unsigned zw = zpw - order_;zw--;) {
            serial_putchar(default_serial_port, _numpadchar);
        }
    }

    _klog_complex_uint_radix_nopad(num, radix);
}

void klog_complex_uint_dec(unsigned long long num) {
    if (_num_padding <= 0) {
        _klog_complex_uint_radix_nopad(num, 10);
    } else {
        _klog_complex_uint_radix_pad(num, 10, _num_padding);
    }
}

void klog_complex_int_dec(long long num) {
    if (num < 0) {
        serial_putchar(default_serial_port, '-');
    }
    klog_complex_uint_dec(num);
}

void klog_complex_uint_hex(unsigned long long num) {
    serial_putchar(default_serial_port, '0');
    serial_putchar(default_serial_port, 'x');
    if (_num_padding <= 0) {
        _klog_complex_uint_radix_nopad(num, 16);
    } else {
        _klog_complex_uint_radix_pad(num, 16, _num_padding);
    }
}

void klog_complex_int_hex(long long num) {
    if (num < 0) {
        serial_putchar(default_serial_port, '-');
    }
    klog_complex_uint_hex(num);
}

void klog_complex_uint_binary(unsigned long long num) {
    serial_putchar(default_serial_port, '0');
    serial_putchar(default_serial_port, 'b');

    if (_num_padding <= 0) {
        _klog_complex_uint_radix_nopad(num, 2);
    } else {
        _klog_complex_uint_radix_pad(num, 2, _num_padding);
    }
}

void klog_complex_int_binary(long long num) {
    if (num < 0) {
        serial_putchar(default_serial_port, '-');
    }
    klog_complex_uint_binary(num);
}

void klog_complex_uint_octal(unsigned long long num) {
    serial_putchar(default_serial_port, '0');
    serial_putchar(default_serial_port, 'o');

    if (_num_padding <= 0) {
        _klog_complex_uint_radix_nopad(num, 8);
    } else {
        _klog_complex_uint_radix_pad(num, 8, _num_padding);
    }
}

void klog_complex_int_octal(long long num) {
    if (num < 0) {
        serial_putchar(default_serial_port, '-');
    }
    klog_complex_uint_octal(num);
}
