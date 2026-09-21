#include <stdio.h>
#include <stdlib.h>
#include "tipos.h"
#include "cpu.h"
#include "memoria.h"
#include "loader.h"
#include "dissasembler.h"



int main(int argc, char *argv[]){

    if(argc < 2){
        printf("Error: Uso incorrecto. \n");
        printf("Uso: %s <archivo.vmx> [-d]\n", argv[0]);
        return 1;
    }

    char *ruta_archivo = argv[1];

    printf("Iniciando MV\n");



    incicializar_registros();


    //cargar_programa(ruta_archivo);

    if(argc >= 3 && strcmp(argv[2], "-d") == 0){
        dword ip_actual = 0;

        while(ip_actual < tamano_codigo){
            ip_actual = desensamblar(ip_actual);
        }
    }

    iniciar_ejecucion();


    printf("Apagando MV\n");
    return 0;
}
