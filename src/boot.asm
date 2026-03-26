MAGIC       equ     0xE85250D6
MBARCH      equ     0
MBLEN       equ     mb_end - mb_start
CHECKSUM    equ     -(MAGIC + MBARCH + MBLEN)

section .multiboot
align 4
mb_start:
    dd MAGIC
    dd MBARCH
    dd MBLEN
    dd CHECKSUM

mb_tag_null:
    dw 0
    dw 0
    dd 8
mb_end:

section .bss
align 16
stack_bottom:
resb 16384
stack_top:

section .text
global _start:function (_start.end - _start)
_start:
    mov esp, stack_top
    extern kernel_main
    call kernel_main

    cli
.hang:
    hlt
    jmp .hang
.end:
