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
    if (argc < 2) {
        printf("Error: Falta el archivo .vmx\n");
        return 1;
    }

    char *rutaArchivo = argv[1];
    
    bool modo_disassembler = false;
    bool parametrosflag = false;
    int cant_parametros = 0;
    int i_parametros;

    for (int i = 1; i < argc; i++) {
        if (strncmp(argv[i], "m=", 2) == 0) {
            TAMANO_MEMORIA = atoi(argv[i] + 2);
            printf("Memoria ajustada a %d bytes.\n", TAMANO_MEMORIA);
        }else 
            if (strcmp(argv[i], "-d") == 0) {
                modo_disassembler = true;
        }else
            if (strcmp(argv[i], "-p") == 0) {
            i_parametros = i + 1; // Guardamos la posicion del primer parametro
            cant_parametros = argc - i_parametros; // Calculamos cuantos arg quedan
            parametrosflag = true;
            
            break;
            }
    }
    
    char *parametros[] = NULL;
    if (parametrosflag) {
        parametros = &argv[i_parametros];
    }


    // se inicial memoria antes de cargar los segmentos con el archivo
    inicializar_memoria();

    if (!cargarPrograma(rutaArchivo, cant_parametros, parametros)) {
        printf("Fallo la carga del loader.\n");
        return 1; 
    }

    word tamano_codigo = obtenerTamCodigo();

    if (modo_disassembler) {
        dword ip_actual = 0;

        while(ip_actual < tamano_codigo){
            ip_actual = desensamblar(ip_actual);
        }
    }

    inicializar_registros();
    
    iniciar_ejecucion();


    return 0;
}