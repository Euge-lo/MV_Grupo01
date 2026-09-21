#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"

byte memoria_principal[TAMANO_MEMORIA] = {0};

dword tabla_segmentos[8] = {0};

void inicializar_memoria(void){
    for(int i = 0; i < TAMANO_MEMORIA; i++)
        memoria_principal[i] = 0;
}

void configurar_segmentos(word tamano_codigo) {

    tabla_segmentos[0] = (0x0000 << 16) | (tamano_codigo & 0xFFFF);


    word base_datos = tamano_codigo;
    word tamano_datos = 16384 - tamano_codigo;
    tabla_segmentos[1] = (base_datos << 16) | (tamano_datos & 0xFFFF);


    for (int i = 2; i < 8; i++) {
        tabla_segmentos[i] = 0xFFFFFFFF;
    }
}


dword traducir_direccion(dword direccion_logica, byte cant_bytes){
    word indice_segmento = (direccion_logica >> 16) & 0xFFFF; //nos quedamos con los 16 bits mas significativos


    word desplazamiento = direccion_logica & 0xFFFF; // nos quedamos con los 16 menos significativos


    if(indice_segmento >= 8){
        printf("Error en el segmento\n");
        exit(1);
    }



    dword base = tabla_segmentos[indice_segmento];

    dword tamano_segmento = base & 0xFFFF;
    dword base_fisica = (base >> 16) & 0xFFFF;
    dword direccion_fisica = base_fisica + desplazamiento;

    dword limite_segmento = base_fisica + tamano_segmento;

    dword limite_acceso = direccion_fisica + cant_bytes;

    if (direccion_fisica >= TAMANO_MEMORIA || limite_acceso > TAMANO_MEMORIA) {
        printf("Error: La direccion física excede la RAM total de la VM.\n");
        exit(1);
    }

    if (!(base_fisica <= direccion_fisica && limite_segmento >= limite_acceso)) {
        printf("Error: Segmentation Fault. Acceso fuera del segmento.\n");
        exit(1);
    }


    return direccion_fisica;
}

byte leer_memoria_byte(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica, 1);
    return memoria_principal[dir_fisica];
}

dword leer_memoria_dword(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica, 4);



    byte b1 = memoria_principal[dir_fisica];
    byte b2 = memoria_principal[dir_fisica + 1];
    byte b3 = memoria_principal[dir_fisica + 2];
    byte b4 = memoria_principal[dir_fisica + 3];

    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

void escribir_memoria_byte(dword direccion_logica, byte valor) {

    dword dir_fisica = traducir_direccion(direccion_logica, 1);
    memoria_principal[dir_fisica] = valor;
}

void escribir_memoria_dword(dword direccion_logica, dword valor) {

    dword dir_fisica = traducir_direccion(direccion_logica, 4);


    memoria_principal[dir_fisica] = (valor >> 24) & 0xFF;
    memoria_principal[dir_fisica + 1] = (valor >> 16) & 0xFF;
    memoria_principal[dir_fisica + 2] = (valor >> 8) & 0xFF;
    memoria_principal[dir_fisica + 3] = valor & 0xFF;
}
