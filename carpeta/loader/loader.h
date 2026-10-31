#ifndef LOADERH
#define LOADERH

#include <stdbool.h>
#include "tipos.h" 
#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo, int cant_parametros, char *parametros[]);
word get_entry_point(void);
bool generar_imagen_vmi(const char *rutaArchivo, dword registros[32], dword tabla_segmentos[8], byte *memoria, word tamano_memoria_kib);
bool cargar_imagen_vmi(const char *rutaArchivo, dword registros[32], dword tabla_segmentos[8], byte *memoria, word *tamano_memoria_kib);

#endif