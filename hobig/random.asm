; ----------------------------------------------------------------------------------------
;     nasm -felf64 hello.asm
; ----------------------------------------------------------------------------------------

global random_64bit_integer
global random_integer
global clock_counter
global random_s64
global is_pow2

section .text
random_64bit_integer:
    rdseed rax
    ret

random_s64:
random_integer:
    rdseed rax      ; R
    ;rdrand rax
    xor rdx, rdx
    sub rsi, rdi    ; max = max - min
    div rsi         ; R % max
    mov rax, rdx    ; modulo
    add rax, rdi    ; min + (R % max)
    ret

clock_counter:
    xor rax, rax
    rdtsc
    shl rdx, 32
    or rax, rdx
    ret

is_pow2:
    blsr rax, rdi
    ret