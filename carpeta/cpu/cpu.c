#include <stdio.h>
#include "cpu.h"

// Creación física del estado del procesador
dword registros[32] = {0}; 


void inicializar_registros(void) {
    
    for (int i = 0; i < 32; i++) {
        registros[i] = 0;
    }

    
    // CS: Índice 0, Offset 0
    registros[CS] = 0x00000000; 

    // DS: Índice 1, Offset 0 
    registros[DS] = (1 << 16) | 0x0000; 

    
    registros[IP] = registros[CS]; 
    
    printf("Procesador inicializado. CS: %08X | DS: %08X | IP: %08X\n", 
           registros[CS], registros[DS], registros[IP]);
}