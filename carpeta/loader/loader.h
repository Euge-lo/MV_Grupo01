#ifndef LOADERH
#define LOADERH

#include <stdbool.h>

#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo);

word obtenerTamanoCodigo(void);

#endif