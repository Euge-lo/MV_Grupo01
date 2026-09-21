#ifndef MEMORIA_H
#define MEMORIA_H

#include "tipos.h"

#define TAMANO_MEMORIA 16384

void inicializar_memoria(void);
dword traducir_direccion(dword direccion_logica);
byte leer_memoria_byte(dword direccion_logica);
dword leer_memoria_dword(dword direccion_logica);

#endif // MEMORIA_H
