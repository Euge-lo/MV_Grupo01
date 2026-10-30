#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "loader/loader.h"
#include "loader/loader.c"
#include "memoria/memoria.h"
#include "memoria/memoria.c"
#include "tipos.h"
#include "dissasembler.h"
#include "dissasembler.c"
#include "instrucciones.c"
#include "instrucciones.h"
#include "cpu/cpu.h"
#include "cpu/cpu.c"

int main(int argc, char *argv[]) {

    // Me fijo si esta el vmi, el vmx, ninguno o ambos
    char *archivo_vmx = NULL;
    char *archivo_vmi = NULL;

    for (int i = 1; i < argc; i++) {
        if (strstr(argv[i], ".vmx") != NULL) {
            archivo_vmx = argv[i];
        } else if (strstr(argv[i], ".vmi") != NULL) {
            archivo_vmi = argv[i];
        }
    }

    if (archivo_vmx == NULL && archivo_vmi == NULL) {
        printf("Error: Es obligatorio indicar al menos un archivo .vmx o .vmi.\n");
        return 1;
    }

    
    bool modo_disassembler = false;
    bool parametrosflag = false;
    int cant_parametros = 0;
    int i_parametros;

    
    for (int i = 1; i < argc; i++) {
            if (strncmp(argv[i], "m=", 2) == 0) {
                TAMANO_MEMORIA = atoi(argv[i] + 2);
                printf("Memoria ajustada a %d bytes.\n", TAMANO_MEMORIA);
            } 
            else 
                if (strcmp(argv[i], "-d") == 0) {
                modo_disassembler = true;
                } 
                else 
                    if (strcmp(argv[i], "-p") == 0) {
                        //En caso de no especificarse un .vmx, se ignoran los parametros -p
                        if (archivo_vmx != NULL) {
                            i_parametros = i + 1; 
                            cant_parametros = argc - i_parametros; 
                            parametrosflag = true;
                        } else {
                            printf("Aviso: No se especifico archivo .vmx, se ignoran los parametros -p \n");
                        }
                        break;
                    }
    }


    
    char **parametros = NULL;
    if (parametrosflag) {
        parametros = &argv[i_parametros];
    }


    // se inicial memoria antes de cargar los segmentos con el archivo
    inicializar_memoria();

    if (archivo_vmx != NULL) {
        if (!cargarPrograma(archivo_vmx, cant_parametros, parametros)) {
            printf("Fallo la carga del loader (.vmx).\n");
            return 1; 
        }
    } 
    else
        if (archivo_vmi != NULL) {
            printf("Reanudando ejecucion desde la imagen: %s\n", archivo_vmi);

            byte *puntero_mem = obtener_puntero_memoria();
            dword *puntero_seg = obtener_tabla_segmentos();
            dword *puntero_reg = obtener_puntero_registros();
            word nuevo_tamano_kib;

            if (!cargar_imagen_vmi(archivo_vmi, puntero_reg, puntero_seg, puntero_mem, &nuevo_tamano_kib)) {
                printf("Error al reanudar la maquina virtual.\n");
                return 1;
            }
        }

    

    word tamano_codigo = obtenerTamCodigo();

    if (modo_disassembler) {
        dword ip_actual = 0;

        while(ip_actual < tamano_codigo){
            ip_actual = desensamblar(ip_actual);
        }
    }

    if (archivo_vmx != NULL) {
        inicializar_registros();
    }
    
    iniciar_ejecucion(archivo_vmi);


    return 0;
}