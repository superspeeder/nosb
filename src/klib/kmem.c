//
// Created by andy on 4/8/26.
//

#include "kmem.h"

void *memset(void *s, int c, size_t n) {
    register unsigned char *s_ = s;
    for (int i = 0; i < n; i++) {
        s_[i] = c;
    }
}

void *memcpy(void *dest, const void *src, size_t n) {
    register unsigned char       *d = dest;
    register const unsigned char *s = src;
    for (int i = 0; i < n; i++) {
        d[i] = s[i];
    }
}

int memcmp(const void *s1, const void *s2, size_t n) {
    register unsigned char *s1_ = s1;
    register unsigned char *s2_ = s2;
    for (int i = 0; i < n; i++) {
        if (s1_[i] != s2_[i]) {
            return s1_[i] > s2_[i] ? 1 : -1;
        }
    }
    return 0;
}
