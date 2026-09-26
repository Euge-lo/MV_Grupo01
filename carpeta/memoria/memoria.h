#include "..\tipos.h"

#define TAMANO_MEMORIA 16384

void configurar_segmentos(word tamano_codigo);
void inicializar_memoria(void);
dword traducir_direccion(dword direccion_logica, byte cant_bytes);
byte leer_memoria_byte(dword direccion_logica);
dword leer_memoria_dword(dword direccion_logica);
void escribir_memoria_byte(dword direccion_logica, byte valor);
void escribir_memoria_dword(dword direccion_logica, dword valor);