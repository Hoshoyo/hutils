global div_word
global mul_word
global add_u64
global sub_u64

section .text

; RDI, RSI, RDX, RCX, R8, R9

; u64 div_word(u64 dividend_high, u64 dividend_low, u64 divisor, u64* out_remainder)

div_word:
    mov r8, rdx ; divisor

    mov rdx, rdi
    mov rax, rsi

    div r8

    mov [rcx], rdx
    ; quotient in rax

    ret

; u64 mul_word(u64 val1, u64 val2, u64* higher)
mul_word:
    mov rcx, rdx
    mov rax, rdi
    mul rsi
    mov [rcx], rdx

    ret

; u64 add_u64(u64 x, u64 y, u64 carry, u64* result)
; returns carry
add_u64:
    xor rax, rax
    add rdi, rdx ; x + c
    setc al
    add rsi, rdi ; (x + c) + y
    setc r8b
    mov [rcx], rsi
    or al, r8b
    ret

; u64 sub_u64(u64 x, u64 y, u64 carry, u64* result)
; returns carry
sub_u64:
    xor rax, rax
    sub rdi, rdx
    setc r8b
    sub rdi, rsi
    setc al
    mov [rcx], rdi
    or al, r8b
    ret