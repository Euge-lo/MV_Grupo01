#ifndef LOADERH
#define LOADERH

#include <stdbool.h>

// Inclui el archivo donde tengas definido el tipo 'word' para que el compilador lo reconozca
#include "../memoria/memoria.h"

bool cargarPrograma(const char *rutaArchivo);

word obtenerTamanoCodigo(void);

#endif