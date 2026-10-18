#include <stdio.h>
#include <stdlib.h>
#include "memoria.h"
#include "..\cpu\cpu.h"

byte memoria_principal[TAMANO_MEMORIA] = {0};

dword tabla_segmentos[8] = {0};

void inicializar_memoria(void){
    for(int i = 0; i < TAMANO_MEMORIA; i++)
        memoria_principal[i] = 0;
}

void configurar_segmentos(dword t_PS, dword t_KS, dword t_CS, dword t_DS, dword t_ES, dword t_SS) {
    dword base_fisica = 0;
    dword indice_tabla = 0;

    dword tamanos[6] = {t_PS, t_KS, t_CS, t_DS, t_ES, t_SS};
    dword reg_ids[6] = {PS, KS, CS, DS, ES, SS};

    for(int i = 0; i < 6; i++){
        if(tamanos[i] > 0){
            if(base_fisica + tamanos[i] > TAMANO_MEMORIA){
                printf("ERROR: Memoria insuficiente.\n");
                exit(1);
            }

            tabla_segmentos[indice_tabla] = (base_fisica << 16) | tamanos[i] & 0xFFFF;
            registros[reg_ids[i]] = (indice_tabla << 16) | 0x0000;

            base_fisica += tamanos[i];
            indice_tabla++;
        }else{
            registros[reg_ids[i]] = 0xFFFFFFFF;
        }
    }

    for(int i = indice_tabla; i < 8; i++)
        tabla_segmentos[i] = 0xFFFFFFFF;

    if(t_SS > 0)
        registros[SP] = registros[SS] + t_SS;
    else
        registros[SP] = 0xFFFFFFFF;
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

word leer_memoria_word(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica, 2);

    registros[MAR] = (2 << 16) | (dir_fisica & 0xFFFF); //tamaño en la parte alta, dirFísica en la baja

    byte b1 = memoria_principal[dir_fisica];
    byte b2 = memoria_principal[dir_fisica + 1];

    return (b1 << 8) | b2;
}


dword leer_memoria_dword(dword direccion_logica){
    dword dir_fisica = traducir_direccion(direccion_logica, 4);

    registros[MAR] = (4 << 16) | (dir_fisica & 0xFFFF); //4 por la cant de bytes a guardar

    byte b1 = memoria_principal[dir_fisica];
    byte b2 = memoria_principal[dir_fisica + 1];
    byte b3 = memoria_principal[dir_fisica + 2];
    byte b4 = memoria_principal[dir_fisica + 3];

    return (b1 << 24) | (b2 << 16) | (b3 << 8) | b4;
}

void escribir_memoria_byte(dword direccion_logica, byte valor) {

    dword dir_fisica = traducir_direccion(direccion_logica, 1);
    registros[MAR] = (1 << 16) | (dir_fisica & 0xFFFF);
    memoria_principal[dir_fisica] = valor;
}

void escribir_memoria_word(dword direccion_logica, word valor){
    dword dir_fisica = traducir_direccion(direccion_logica, 2);
    registros[MAR] = (2 << 16) | (dir_fisica & 0xFFFF);

    memoria_principal[dir_fisica] = (valor >> 8) & 0xFF;
    memoria_principal[dir_fisica + 1] = valor & 0xFF;
}

void escribir_memoria_dword(dword direccion_logica, dword valor) {

    dword dir_fisica = traducir_direccion(direccion_logica, 4);

    registros[MAR] = (4 << 16) | (dir_fisica & 0xFFFF); //4 por la cant de bytes a guardar

    memoria_principal[dir_fisica] = (valor >> 24) & 0xFF;
    memoria_principal[dir_fisica + 1] = (valor >> 16) & 0xFF;
    memoria_principal[dir_fisica + 2] = (valor >> 8) & 0xFF;
    memoria_principal[dir_fisica + 3] = valor & 0xFF;
}


word obtenerTamCodigo(void) {
    return (word)(tabla_segmentos[0] & 0xFFFF);
}
