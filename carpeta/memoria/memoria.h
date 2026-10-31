#include "..\tipos.h"

extern dword TAMANO_MEMORIA;
extern byte *memoria_principal;

void configurar_segmentos_2(dword t_PS, dword t_KS, dword t_CS, dword t_DS, dword t_ES, dword t_SS);
void configurar_segmentos(word tamano_codigo);
void inicializar_memoria(void);
dword traducir_direccion(dword direccion_logica, byte cant_bytes);
byte leer_memoria_byte(dword direccion_logica);
word leer_memoria_word(dword direccion_logica);
dword leer_memoria_dword(dword direccion_logica);
void escribir_memoria_byte(dword direccion_logica, byte valor);
void escribir_memoria_word(dword direccion_logica, word valor);
void escribir_memoria_dword(dword direccion_logica, dword valor);
word obtenerTamCodigo(void);
byte* obtener_puntero_memoria(void);
dword* obtener_tabla_segmentos(void);
