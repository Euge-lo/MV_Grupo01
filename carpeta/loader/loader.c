#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "loader.h"
#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo) {
    char firma[6] = {0};
    FILE *archivo = fopen(rutaArchivo, "rb");
    byte version;

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s \n", rutaArchivo);
        return false;
    }

    if (fread(firma, 1, 5, archivo) != 5) {
        printf("Error: Archivo demasiado corto.\n");
        fclose(archivo);
        return false;
    }

    if (strncmp(firma, "VMX26", 5) != 0) {
        printf("Error: Firma invalida.\n");
        fclose(archivo);
        return false;
    }

    if (fread(&version, 1, 1, archivo) != 1) {
        printf("Error: Archivo truncado al leer la version.\n");
        fclose(archivo);
        return false;
    }


    if (version == 1) {
        printf("Version validada correctamente, version 1 de la MV \n");
                  //Lectura del Tamano del Codigo
                  byte bufferTam[2];
                  if (fread(bufferTam, 1, 2, archivo) != 2) {
                     printf("Error: Archivo truncado. No se pudo leer el tamano del codigo.\n");
                     fclose(archivo);
                     return false;
                  }

                  word tamCodigo = (bufferTam[0] << 8) | bufferTam[1];
                  configurar_segmentos(tamCodigo);

                  printf("Tamano del bloque de codigo leido: %d bytes \n", tamCodigo);

                     // Volcado del Codigo en el Segmento 0 (Code Segment)
                     dword direccionCodigo = 0x00000000;

                     printf("Cargando %d bytes de codigo en la memoria...\n", tamCodigo);
                     for (int i = 0; i < tamCodigo; i++) {
                        byte byteLeido;
                        // Leemos 1 byte. Si fread no devuelve 1, el archivo esta corrupto o incompleto.
                        if (fread(&byteLeido, 1, 1, archivo) == 1) {
                              // Utilizamos la funcion de escritura de la memoria (asumimos que la creaste en memoria.c)
                              escribir_memoria_byte(direccionCodigo + i, byteLeido);
                        } else {
                              printf("Error: Archivo truncado durante la lectura de las instrucciones.\n");
                              fclose(archivo);
                              return false;
                        }
                     }

                     // Volcado de los Datos en el Segmento 1 (Data Segment)
                     dword direccionDatos = (1 << 16) | 0x0000;
                     dword desplazamiento = 0;
                     byte byteDato;

                     while (fread(&byteDato, 1, 1, archivo) == 1) {
                        escribir_memoria_byte(direccionDatos + desplazamiento, byteDato);
                        desplazamiento++;
                     }

                     printf("Se cargaron %d bytes de variables en el segmento de datos.\n", desplazamiento);

                     fclose(archivo);
                     printf("Carga finalizada con exito. La maquina virtual esta lista \n");
                     return true;
        
    } 
    else 
      if (version == 2) {
        printf("Version validada correctamente, version 2 de la MV\n");
        
        // =========================================================
        // ACÁ VA LA LÓGICA NUEVA PARA EL TP 2
        // =========================================================
        
        // 1. Leer los 12 bytes con los tamanos de PS, KS, CS, DS, ES, SS y el entry point.
        // 2. Extraer los datos con corrimientos de bits.
        // 3. Informar a memoria.c para que aloje el espacio dinamico.
        // 4. Leer el archivo y volcar unicamente en CS y KS.
        
        fclose(archivo);
        return true;
    } 
      else {
        printf("Error: Version incompatible (%d).\n", version);
        fclose(archivo);
        return false;
    }
}
