[section .multiboot]
[bits 32]
MBMAGIC equ 0xE85250D6
MBARCH  equ 0
MBHLEN  equ mbend - mbstart
MBCHECK equ -(MBMAGIC + MBARCH + MBHLEN)

mbstart:
align 4
    dd MBMAGIC
    dd MBARCH
    dd MBHLEN
    dd MBCHECK

fbstart:
    dd 5
    dd fbend - fbstart
    dd 1024
    dd 768
    dd 32
fbend:
    dd 0
    dd 0
mbend:


[section .text]
[bits 32]
PRESENT                 equ 1 << 7
NOT_SYS                 equ 1 << 4
EXEC                    equ 1 << 3
RW                      equ 1 << 1
GRAN_4K                 equ 1 << 7
SZ_32                   equ 1 << 6
LONG_MODE               equ 1 << 5
CPUID_EXTENSIONS        equ 0x80000000
CPUID_EXT_FEATURES      equ 0x80000001
CPUID_EDX_EXT_FEAT_LM   equ 1 << 29
CR0_PAGING              equ 1 << 31
EFLAGS_ID               equ 1 << 21
CPUID_GET_FEATURES      equ 7
CPUID_FEATURE_PML5      equ 1 << 16 
CR4_LA57                equ 1 << 12

global _start
_start:
    ; We start in 32-bit mode
    ; We should check that eax contains 0x36d76289
    ; We should store the value of ebx for later
    
    mov esp, stack_top
    mov ebp, esp


    mov [mbi_ptr], ebx

    mov ebx, 0x36d76289
    cmp eax, ebx
    jne halt32
    
    cli

    ; Check if long mode is supported
    call checkCPUID
    jnz .hasCPUID

.noCPUID:
    hlt
    jmp .noCPUID

.hasCPUID:
    ; We can call cpuid
.queryLongMode:
    mov eax, CPUID_EXTENSIONS
    cpuid
    cmp eax, CPUID_EXT_FEATURES
    jb noLongMode

    mov eax, CPUID_EXT_FEATURES
    cpuid
    test edx, CPUID_EDX_EXT_FEAT_LM
    jz noLongMode

.disablePaging32:
    mov eax, cr0
    and eax, ~CR0_PAGING
    mov cr0, eax

.setupPageTable:
    mov eax, pml4t              ; move the address of the layer-4 page table into eax
    or eax, 0b11                ; set the present and r/w bits on this entry
    mov [pml5t], eax            ; write this value into entry 0 of the layer-5 page table

    mov eax, pdpt               ; move the address of the layer-3 page table into eax
    or eax, 0b11                ; set the present and r/w bits on this entry
    mov [pml4t], eax            ; write this value into entry 0 of the layer-4 page table

    mov eax, pdt                ; move the address of the layer-2 page table into eax
    or eax, 0b11                ; set the present and r/w bits on this entry
    mov [pdpt], eax             ; write this value into entry 0 of the layer-4 page table

    mov eax, pdt2               ; move the address of the layer-2 page table into eax
    or eax, 0b11                ; set the present and r/w bits on this entry
    mov [pdpt + 8], eax         ; write this value into entry 0 of the layer-4 page table

    mov ecx, 0                  ; initialize counter
.ptloop:
    mov eax, 0x200000           ; pages are 2 MiB, so we need to jump by 2 MiB for each page we write
    mul ecx                     ; calculate page address using counter
    or eax, 0b10000011          ; set the present, r/w, huge page
    mov [pdt + ecx * 8], eax    ; set the page table entry in the layer 2 page table (since we use huge pages, there are no layer-1 page tables)

    inc ecx                     ; increment counter
    cmp ecx, 512                ; check if whole table is mapped
    jne .ptloop                 ; if not, continue mapping

    mov ecx, 0                  ; initialize counter
.ptloop2:
    mov eax, 0x200000           ; pages are 2 MiB, so we need to jump by 2 MiB for each page we write
    mul ecx                     ; calculate page address using counter
    or eax, 0b10000011          ; set the present, r/w, huge page
    mov [pdt2 + ecx * 8], eax   ; set the page table entry in the layer 2 page table (since we use huge pages, there are no layer-1 page tables)

    inc ecx                     ; increment counter
    cmp ecx, 512                ; check if whole table is mapped
    jne .ptloop2                ; if not, continue mapping

.enablePaging:

    mov eax, CPUID_GET_FEATURES
    xor ecx, ecx
    cpuid
    test ecx, CPUID_FEATURE_PML5
    jz .4level
    
.5level:
    mov dword [paging_5], 1
    mov eax, pml5t
    mov cr3, eax

    mov eax, cr4
    or eax, CR4_LA57
    mov cr4, eax

    jmp .continue

.4level:
    mov dword [paging_5], 0
    mov eax, pml4t
    mov cr3, eax

.continue:
    ; enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; enable long mode
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax


    lgdt [gdt_descriptor]
    jmp CODE_SEG:finish_setup


; No long mode
noLongMode:
    hlt
    jmp noLongMode


; Check if the cpuid instruction is available.
; This is done by attempting to flip the id bit in the eflags register. if the bit can be flipped, then cpuid is available. Otherwise, it is not.
checkCPUID:
    pushfd
    pop eax
    mov ecx, eax
    xor eax, EFLAGS_ID
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    xor eax, ecx
    jnz .supported
    .notSupported:
        mov ax, 0
        ret
    .supported:
        mov ax, 1
        ret



halt32:
    hlt
    jmp halt32


[bits 64]
extern kernel_main
finish_setup:
    cli
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    xor rdi, rdi
    mov edi, [mbi_ptr]

    call kernel_main

.hang:
    hlt
    jmp .hang


global get_pml4t_addr
get_pml4t_addr:
    mov rax, pml4t
    ret


global get_pml5t_addr
get_pml5t_addr:
    mov rax, pml5t
    ret

global get_cr2
get_cr2:
    mov rax, cr2
    ret


[section .bss]
align 16
stack_bottom:
resb 16384
stack_top:
; Page tables
align 4096
pml5t:
resb 4096
pml4t:
resb 4096
pdpt:
resb 4096
pdt:
resb 4096
pdt2:
resb 4096
; Static Variables
align 4
mbi_ptr: resb 4
global paging_5
paging_5: resb 4


[section .rodata]
gdt_start:
    dq 0
gdt_code:
    dw 0xffff
    dw 0
    db 0
    db PRESENT | NOT_SYS | EXEC | RW
    db GRAN_4K | LONG_MODE | 0xF
    db 0
gdt_data:
    dw 0xffff
    dw 0
    db 0
    db PRESENT | NOT_SYS | RW
    db GRAN_4K | SZ_32 | 0xF
    db 0
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dq gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

