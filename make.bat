lcc -S main.c physics.c
lcc -o ball.gb main.c physics.c mul_q88_asm.s
emulicious ball.gb