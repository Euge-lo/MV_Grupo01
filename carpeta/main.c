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
    // 1. Validar la cantidad de argumentos
    // El programa necesita al menos 2 palabras (vmx y el archivo)
    // y acepta como maximo 3 palabras (si incluye el -d)
   

    if (argc < 2 || argc > 3) {
        printf("Error: Uso incorrecto de la maquina virtual.\n");
        printf("Formato esperado: vmx archivo.vmx [-d]\n");
        return 1;
    }

    // 2. Extraer el nombre del archivo (siempre es la segunda palabra)
    char *rutaArchivo = argv[1];
    

    // 4. Conectar con el Loader
    printf("Intentando cargar: %s\n", rutaArchivo);
    if (!cargarPrograma(rutaArchivo)) {
        return 1;
    }
    else{
      printf("Primer byte del codigo: %02X\n", leer_memoria_byte(0x00000000));
    }

    

    word tamano_codigo = obtenerTamCodigo();

    if(argc >= 3 && strcmp(argv[2], "-d") == 0){
        dword ip_actual = 0;

        while(ip_actual < tamano_codigo){
            ip_actual = desensamblar(ip_actual);
        }
    }

    inicializar_registros();
    
    iniciar_ejecucion();

   printf("Dato en memoria [0]: %08X\n", leer_memoria_dword(0x00010000)); // Lee los 4 bytes enteros
    printf("Dato en memoria [4]: %08X\n", leer_memoria_dword(0x00010004));

    return 0; // Ejecucion exitosa
}