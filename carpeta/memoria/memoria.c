#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"

byte memoria_principal[TAMANO_MEMORIA] = {0};

dword tabla_segmentos[16] = {0};

void inicializar_memoria(void){
    for(int i = 0; i < TAMANO_MEMORIA; i++)
        memoria_principal[i] = 0;
}


dword traducir_direccion(dword direccion_logica){
    word indice_segmento = (direccion_logica >> 16) & 0xFFFF; //nos quedamos con los 16 bits mas significativos


    word desplazamiento = direccion_logica & 0xFFFF; // nos quedamos con los 16 menos significativos

    if(indice_segmento >= 16){
        printf("Error en el segmento\n");
        exit(1);
    }

    dword base_fisica = tabla_segmentos[indice_segmento];
    dword direccion_fisica = base_fisica + desplazamiento;

    if(direccion_fisica >= TAMANO_MEMORIA){
        printf("Error en la direccion fisica\n");
        exit(1);
    }

    return direccion_fisica;
}

byte leer_memoria_byte(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica);
    return memoria_principal[dir_fisica];
}

dword leer_memoria_dword(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica);

    if(dir_fisica + 3 >= TAMANO_MEMORIA){
        printf("Error en la direccion fisica\n");
        exit(1);
    }

    byte b1 = memoria_principal[dir_fisica];
    byte b2 = memoria_principal[dir_fisica + 1];
    byte b3 = memoria_principal[dir_fisica + 2];
    byte b4 = memoria_principal[dir_fisica + 3];

    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}
