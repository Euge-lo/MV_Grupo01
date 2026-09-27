; limite_division.asm
LDL EAX, 0x0000      ; Carga los 16 bits bajos
LDH EAX, 0x8000      ; Carga los 16 bits altos (EAX = 0x80000000, es decir -2,147,483,648)
MOV EBX, 0xFFFFFFFF  ; Divisor: -1 (Funciona directo porque 0xFFFF extiende su signo a 0xFFFFFFFF)
DIV EAX, EBX         ; Provoca overflow de cociente. EAX debe quedar en 0x80000000.

MOV EDX, DS
MOV [EDX], EAX
MOV EAX, 0x08
LDL ECX, 1
LDH ECX, 4
SYS 0x2
STOP