.globl _mul_q88_asm

.area _CODE
; Multiplication of two fixed point 8.8 numbers
; @param bc: operand 1
; @param de: operand 2
; @return bc: new fixed point number
_mul_q88_asm:
    xor a ; We're temporarily holding `l` in `a` for faster calculations.
    ld h, a
    bit 7, d
    jr z, label1
    sub c
label1:
    bit 7, b
    jr z, label2
    sub e
label2:
    ld l, a
    ld a, e ; Holding `e` in `a` instead lets us use shorter (and faster!) instructions.
    ld e, #16
loop:
    add hl, hl
    rla
    rl d
    jr nc, skip
    add hl, bc
    adc a, #0 ; We don't propagate the carry into `d` because we don't care about its final value.
skip:
    dec e
    jr nz, loop
    ; Result in `ah`, put it in `bc`
    ld b, a
    ld c, h
    ret