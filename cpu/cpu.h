#include "..\tipos.h"

//registros de instruccion
#define IP 0
#define OPC 1
#define OP1 2
#define OP2 3

//registros de memoria
#define LAR 4
#define MAR 5
#define MBR 6

//registros generales
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15

//registros de estado
#define AC 16
#define CC 17

//registros de segmentos
#define CS 26
#define DS 27

#define FLAG_N 0x80000000
#define FLAG_Z 0x40000000
#define FLAG_C 0x20000000
#define FLAG_V 0x10000000

extern dword registros[32];

void instruccion_invalida(void);
void inicializar_tabla_funciones(void);
void inicializar_registros(void);
dword leer_valor_operando(byte tipo);
void iniciar_ejecucion(void);

