//
// Created by andy on 4/8/26.
//

#pragma once

void klog_info(const char* msg);
void klog_error(const char* msg);
void klog_warning(const char* msg);
void klog_debug(const char* msg);

void klog_newline();
void klog_begin_complex();
void klog_complex_print(const char* msg);

void klog_reset_fmt_state();
void klog_set_num_pad(unsigned width);

void klog_complex_uint_dec(unsigned long long num);
void klog_complex_int_dec(long long num);
void klog_complex_uint_hex(unsigned long long num);
void klog_complex_int_hex(long long num);
void klog_complex_uint_binary(unsigned long long num);
void klog_complex_int_binary(long long num);
void klog_complex_uint_octal(unsigned long long num);
void klog_complex_int_octal(long long num);
