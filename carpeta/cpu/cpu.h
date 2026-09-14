#define IP  0
#define OPC 1
#define OP1 2
#define OP2 3

// Registros de memoria
#define LAR 4
#define MAR 5
#define MBR 6

// Registros de propósito general
#define EAX 10
#define EBX 11
#define ECX 12
#define EDX 13
#define EEX 14
#define EFX 15

// Registros de estado
#define AC  16
#define CC  17

// Registros de segmentos
#define CS  26
#define DS  27

// ==========================================
// MÁSCARAS PARA EL REGISTRO CC (NZCV)
// ==========================================
#define FLAG_N 0x80000000 // Negativo
#define FLAG_Z 0x40000000 // Cero
#define FLAG_C 0x20000000 // Acarreo
#define FLAG_V 0x10000000 // Desbordamiento

extern dword registros[32]; // Nuestro arreglo de 32 registros de 32 bits


void inicializar_registros(void);