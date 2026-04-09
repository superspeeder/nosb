#include "isr.h"

#include "klib/klog.h"

#include <stdint.h>

static const char* exception_names[32] = {
    "#DE",
    "#DB",
    "NMI",
    "#BP",
    "#OF",
    "#BR",
    "#UD",
    "#NM",
    "#DF",
    "Coprocessor Segment Overrun",
    "#TS",
    "#NP",
    "#SS",
    "#GP",
    "#PF",
    "<unknown>",
    "#MF",
    "#AC",
    "#MC",
    "#XM",
    "#VE",
    "#CP",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
    "<unknown>",
};

__attribute__((noreturn)) void exception_handler(uint8_t interrupt_number) {
    klog_begin_complex();
    klog_set_num_pad(2);
    klog_error("Exception: ");
    klog_complex_print(exception_names[interrupt_number]); // exception_handler should not be used for IRQs
    klog_newline();
    while (1) {
        asm volatile("cli; hlt");
    }
}
