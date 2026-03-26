[org 0x7c00]
[bits 16]
    cli
    lgdt [gdtr]
    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp CODE_SEG:PROTMAIN

[bits 32]
PROTMAIN:
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, 0x90000
    mov esp, ebp

    call BEGIN_PM

BEGIN_PM:
    mov ebx, msg
    call print_string_pm

    jmp $

print_string_pm:
    pusha
    mov edx, VIDEO_MEMORY
.loop:
    mov al, [ebx]
    mov ah, WHITE_ON_BLACK

    cmp al, 0
    je .done

    mov [edx], ax
    add ebx, 1
    add edx, 2

    jmp .loop

.done:
    popa
    ret

;-----------

gdt_start:
gdt_null:
    dq 0
gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0
gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0
gdt_end:

gdtr:
    dw gdt_end - gdt_start - 1
    dd gdt_start

msg db "Hello, World!", 0

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
VIDEO_MEMORY equ 0xb8000
WHITE_ON_BLACK equ 0x0f

times 510-($-$$) db 0
dw 0xaa55

