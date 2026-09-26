; 1. PRUEBA DE CARGA Y ARITMETICA BASICA
MOV EAX, 20          ; Carga inmediato decimal en EAX[cite: 6]
MOV EBX, 0x05        ; Carga inmediato hexadecimal en EBX[cite: 6]
ADD EAX, EBX         ; EAX = 25. Prueba suma[cite: 6]
SUB EAX, 1           ; EAX = 24. Prueba resta[cite: 6]
MUL EAX, 2           ; EAX = 48. Prueba multiplicacion[cite: 6]
DIV EAX, 5           ; EAX = 9. Prueba division. El resto (3) debe ir a AC[cite: 6]

; 2. PRUEBA DE LOGICA Y MANIPULACION DE BITS
SWAP EAX, EBX        ; Intercambia valores: EAX=5, EBX=9[cite: 6]
AND EAX, 0b0111      ; EAX = 5 (0101 AND 0111 = 0101). Prueba AND[cite: 6]
OR EAX, 0x08         ; EAX = 13 (0101 OR 1000 = 1101). Prueba OR[cite: 6]
SHL EAX, 1           ; EAX = 26. Desplaza 1 bit a la izquierda[cite: 6]
XOR EAX, EAX         ; EAX = 0. Prueba XOR y limpieza de registro[cite: 6]
NOT EAX              ; EAX = 0xFFFFFFFF (-1). Invierte todos los bits[cite: 6]

; 3. PRUEBA DE MEMORIA Y DESPLAZAMIENTOS
MOV EDX, DS          ; EDX apunta a la base del Data Segment[cite: 6]
MOV [EDX], 0x4F      ; Guarda la letra 'O' en la primera celda de datos[cite: 6]
MOV [EDX+4], 0x4B    ; Guarda la letra 'K' desplazandose 4 bytes[cite: 6]

; 4. PRUEBA DE SALTOS Y CONDICIONALES
MOV ECX, 3           ; Setea un contador en 3[cite: 6]
bucle: CMP ECX, 0    ; Compara el contador con 0, afecta al registro CC[cite: 6]
JZ fin               ; Si es cero (Z=1), salta a fin[cite: 6]
SUB ECX, 1           ; Resta 1 al contador[cite: 6]
JMP bucle            ; Salto incondicional al inicio del bucle[cite: 6]

; 5. PRUEBA DE LLAMADA AL SISTEMA (SYS WRITE)
fin: MOV EAX, 0x08   ; Configura EAX para escribir en modo caracteres[cite: 6]
LDL ECX, 2           ; 2 bytes menos significativos: Imprimir 2 valores[cite: 6]
LDH ECX, 4           ; 2 bytes mas significativos: Tamanos de 4 bytes[cite: 6]
SYS 0x2              ; Llamada al sistema 2 (WRITE) usando la direccion en EDX[cite: 6]

STOP                 ; Detiene la ejecucion del proceso[cite: 6]