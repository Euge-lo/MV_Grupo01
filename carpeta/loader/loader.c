#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include "loader.h"
#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo) {
    char firma[6] = {0}; // VMX26
    FILE *archivo = fopen(rutaArchivo, "rb");
    byte version;

    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo %s \n", rutaArchivo);
        return false;
      }
    else{

      if (fread(firma, 1, 5, archivo) != 5) {
            printf("Error: Archivo demasiado corto para leer la firma.\n");
            fclose(archivo);
            return false;
        }

       // Validacion del Identificador (Bytes 0-4: "VMX26")
      if (strncmp(firma, "VMX26", 5) != 0) {
         printf("Error: Firma invalida. Se esperaba VMX26, se leyo %s\n", firma);
         fclose(archivo);
         return false;
      }
      else{
         printf("Firma VMX26 validada correctamente \n");
            //Validacion de la Version (Byte 5: Valor 1)
            // Leemos 1 bloque de 1 byte
            if (fread(&version, 1, 1, archivo) != 1) {
               printf("Error: Archivo truncado. No se pudo leer la version.\n");
               fclose(archivo);
               return false;
            }
            else{
               if (version != 1) {
                  printf("Error: Version incompatible (%d). Se esperaba la version 1.\n", version);
                  fclose(archivo);
                  return false;
               }
               else{
                  printf("Version validada correctamente.\n");
                  //Lectura del Tamano del Codigo (Bytes 6-7)
                  byte bufferTam[2];
                  // Leemos 2 bloques de 1 byte
                  if (fread(bufferTam, 1, 2, archivo) != 2) {
                     printf("Error: Archivo truncado. No se pudo leer el tamano del codigo.\n");
                     fclose(archivo);
                     return false;
                  }

                  // Reconstruimos el numero de 16 bits usando algebra de bits (Big-Endian)
                  word tamCodigo = (bufferTam[0] << 8) | bufferTam[1];
                  configurar_segmentos(tamCodigo);


                  printf("Tamano del bloque de codigo leido: %d bytes \n", tamCodigo);

                  // Parte sin revisar todavia


                     // 5. Volcado del Codigo en el Segmento 0 (Code Segment)
                     // La direccion base es 0x00000000 (Segmento 0, Offset 0)
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
                     // Construimos la direccion base 0x00010000 (Segmento 1, Offset 0)
                     dword direccionDatos = (1 << 16) | 0x0000;
                     dword desplazamiento = 0;
                     byte byteDato;

                     while (fread(&byteDato, 1, 1, archivo) == 1) {
                        escribir_memoria_byte(direccionDatos + desplazamiento, byteDato);
                        desplazamiento++;
                     }

                     printf("Se cargaron %d bytes de variables en el segmento de datos.\n", desplazamiento);

                     // Cierre y senal de arranque
                     // Liberamos el archivo fisico y le devolvemos el control a main.c
                     fclose(archivo);
                     printf("Carga finalizada con exito. La maquina virtual esta lista \n");
                     return true;

               }



            }




         }



      }

}

