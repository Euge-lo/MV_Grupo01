#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "loader.h"
#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo, int cant_parametros, char *parametros[]){
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
        
        //Leer los 12 bytes de la cabecera
        byte bufferV2[12];
        if (fread(bufferV2, 1, 12, archivo) != 12) {
            printf("Error: Archivo truncado al leer la cabecera de la version 2.\n");
            fclose(archivo);
            return false;
        }

        //Extraer los datos
        word cs_size     = (bufferV2[0] << 8)  | bufferV2[1];
        word ds_size     = (bufferV2[2] << 8)  | bufferV2[3];
        word es_size     = (bufferV2[4] << 8)  | bufferV2[5];
        word ss_size     = (bufferV2[6] << 8)  | bufferV2[7];
        word ks_size     = (bufferV2[8] << 8)  | bufferV2[9];
        word entry_point = (bufferV2[10] << 8) | bufferV2[11];

        // chwqueo de tamanos
        // printf("Tamanos v2 -> CS: %d, DS: %d, ES: %d, SS: %d, KS: %d | Entry: %d\n", cs_size, ds_size, es_size, ss_size, ks_size, entry_point);
        word ps_size = 0;
        if (cant_parametros > 0 && parametros != NULL) {
            for (int i = 0; i < cant_parametros; i++) {
                // strlen cuenta las letras, el +1 es para contar el '\0'
                ps_size += strlen(parametros[i]) + 1; 
            }
        }
        // Configurar tabla de segmentos
        configurar_segmentos_v2(cs_size, ds_size, es_size, ss_size, ks_size, ps_size);

        dword direccionCS = (2 << 16) | 0x0000;
        dword direccionKS = (1 << 16) | 0x0000;

        //Volcado del Code Segment (CS)
        if (cs_size > 0) {
            printf("Cargando %d bytes en el Code Segment...\n", cs_size);
            for (int i = 0; i < cs_size; i++) {
                byte byteLeido;
                if (fread(&byteLeido, 1, 1, archivo) == 1) {
                    escribir_memoria_byte(direccionCS + i, byteLeido);
                } else {
                    printf("Error: Archivo truncado al leer el Code Segment.\n");
                    fclose(archivo);
                    return false;
                }
            }
        }

        // Volcado del Const Segment (KS)
        if (ks_size > 0) {
            printf("Cargando %d bytes en el Const Segment...\n", ks_size);
            for (int i = 0; i < ks_size; i++) {
                byte byteLeido;
                if (fread(&byteLeido, 1, 1, archivo) == 1) {
                    escribir_memoria_byte(direccionKS + i, byteLeido);
                } else {
                    printf("Error: Archivo truncado al leer el Const Segment.\n");
                    fclose(archivo);
                    return false;
                }
            }
        }

        //Volcado del param segment (PS)
        if (cant_parametros > 0 && parametros != NULL) {
            dword direccionPS = (0 << 16) | 0x0000; // indice 0
            dword offset_ps = 0;

            printf("Volcando parametros en el Param Segment...\n");

            // Recorremos el arreglo de strings que nos paso el main
            for (int i = 0; i < cant_parametros; i++) {
                // Volcamos letra por letra
                for (int j = 0; parametros[i][j] != '\0'; j++) {
                    escribir_memoria_byte(direccionPS + offset_ps, parametros[i][j]);
                    offset_ps++;
                }
                // Terminador nulo de C
                escribir_memoria_byte(direccionPS + offset_ps, '\0');
                offset_ps++;
            }
            printf("Se volcaron %d bytes de texto en el Param Segment.\n", offset_ps);
        }

        printf("Carga de Version 2 finalizada con exito.\n");
        fclose(archivo);
        return true;
    } 
      else {
        printf("Error: Version incompatible (%d).\n", version);
        fclose(archivo);
        return false;
    }
}

bool generar_imagen_vmi(const char *rutaArchivo, dword registros[32], dword tabla_segmentos[8], byte *memoria, word tamano_memoria_kib) {
    FILE *archivo = fopen(rutaArchivo, "wb");
    if (archivo == NULL) {
        printf("Error: No se pudo crear el archivo de imagen %s\n", rutaArchivo);
        return false;
    }

    //Escritura del Header (8 bytes totales)
    // Bytes 0-4: Identificador
    fwrite("VMI26", 1, 5, archivo);
    
    // Byte 5: Version
    byte version = 1;
    fwrite(&version, 1, 1, archivo);
    
    // Bytes 6-7: memoria en KiB
    // Desplazamiento de bits para asegurar correcta lectura cruzada
    byte bufferTam[2];
    bufferTam[0] = (tamano_memoria_kib >> 8) & 0xFF;
    bufferTam[1] = tamano_memoria_kib & 0xFF;
    fwrite(bufferTam, 1, 2, archivo);

    //Escritura de Registros
    if (fwrite(registros, sizeof(dword), 32, archivo) != 32) {
        printf("Error al escribir los registros en el archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    // Tabla de descriptores de segmentos (8 x 4 = 32 bytes)
    if (fwrite(tabla_segmentos, sizeof(dword), 8, archivo) != 8) {
        printf("Error al escribir la tabla de segmentos en el archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    // Memoria principal (variable)
    // Convertimos los KiB a bytes reales para saber cuanto volcar
    dword tamano_memoria_bytes = tamano_memoria_kib * 1024;
    if (fwrite(memoria, 1, tamano_memoria_bytes, archivo) != tamano_memoria_bytes) {
        printf("Error al escribir la memoria en el archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    fclose(archivo);
    printf("Imagen generada correctamente: %s (Memoria volcada: %d KiB)\n", rutaArchivo, tamano_memoria_kib);
    return true;
}

// IMPORTANTE: SE NECESITA TENER LA RAM YA EXISTA (MALLOC PREVIO) PARA EJECUTAR cargar_imagen_vmi
bool cargar_imagen_vmi(const char *rutaArchivo, dword registros[32], dword tabla_segmentos[8], byte *memoria, word *tamano_memoria_kib) {
    FILE *archivo = fopen(rutaArchivo, "rb"); // "rb" para lectura binaria
    if (archivo == NULL) {
        printf("Error: No se pudo abrir el archivo de imagen %s\n", rutaArchivo);
        return false;
    }

    // Lectura y validacion del Header (8 bytes)
    char firma[6] = {0};
    if (fread(firma, 1, 5, archivo) != 5 || strncmp(firma, "VMI26", 5) != 0) {
        printf("Error: Firma invalida en el archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    byte version;
    if (fread(&version, 1, 1, archivo) != 1 || version != 1) {
        printf("Error: Version de VMI incompatible.\n");
        fclose(archivo);
        return false;
    }

    byte bufferTam[2];
    if (fread(bufferTam, 1, 2, archivo) != 2) {
        printf("Error: Archivo truncado al leer tamaño de memoria.\n");
        fclose(archivo);
        return false;
    }
    
    // Reconstruimos el tamano en KiB
    *tamano_memoria_kib = (bufferTam[0] << 8) | bufferTam[1];
    dword tamano_memoria_bytes = (*tamano_memoria_kib) * 1024;

    // Lectura de Registros (128 bytes)
    if (fread(registros, sizeof(dword), 32, archivo) != 32) {
        printf("Error al leer los registros del archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    // Tabla de descriptores de segmentos (32 bytes)
    if (fread(tabla_segmentos, sizeof(dword), 8, archivo) != 8) {
        printf("Error al leer la tabla de segmentos del VMI.\n");
        fclose(archivo);
        return false;
    }

    // Memoria principal
    if (fread(memoria, 1, tamano_memoria_bytes, archivo) != tamano_memoria_bytes) {
        printf("Error al leer la memoria del archivo VMI.\n");
        fclose(archivo);
        return false;
    }

    fclose(archivo);
    printf("Imagen cargada con exito: %s (Memoria restaurada: %d KiB)\n", rutaArchivo, *tamano_memoria_kib);
    return true;
}
