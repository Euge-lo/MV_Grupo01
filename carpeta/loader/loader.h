#ifndef LOADERH
#define LOADERH

#include <stdbool.h>

#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo, int cant_parametros, char **parametros);

#endif