//
// Created by andy on 4/8/26.
//

#pragma once
#include <stdint.h>

void pic_init();
void pic_send_eoi(uint8_t irq);
void pic_irq_set_mask(uint8_t irq_line);
void pic_irq_clear_mask(uint8_t irq_line);
void pic_remap(int offset1, int offset2);