#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "loader/loader.h"
#include "loader/loader.c"
#include "memoria/memoria.h"
#include "memoria/memoria.c"
#include "tipos.h"

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


    return 0; // Ejecucion exitosa
}