#include <stdio.h>
#include "cpu.h"
#include "../memoria/memoria.h"

typedef void (*Instruccion)(void);

Instruccion tabla_funciones[32];

void instruccion_invalida(void){
    printf("Instruccion no registrada (OPC: %02X)\n", registros[OPC]);
}

void inicializar_tabla_funciones(void){
    for(int i = 0; i < 32; i++){
        tabla_funciones[i] = instruccion_invalida;
    }

    tabla_funciones[0x10] = MOV;
    tabla_funciones[0x11] = ADD;
    tabla_funciones[0x12] = SUB;
    tabla_funciones[0x13] = MUL;
    tabla_funciones[0x14] = DIV;
    tabla_funciones[0x15] = CMP;
    tabla_funciones[0x16] = AND;
    tabla_funciones[0x17] = OR;
    tabla_funciones[0x18] = XOR;
    tabla_funciones[0x19] = SWAP;
    tabla_funciones[0x1A] = SHL;
    tabla_funciones[0x1B] = SHR;
    tabla_funciones[0x1C] = SAR;
    tabla_funciones[0x1D] = LDL;
    tabla_funciones[0x1E] = LDH;
    tabla_funciones[0x1F] = RND;
    tabla_funciones[0x00] = SYS;
    tabla_funciones[0x01] = JMP;
    tabla_funciones[0x02] = JP;
    tabla_funciones[0x03] = JN;
    tabla_funciones[0x04] = JZ;
    tabla_funciones[0x05] = JC;
    tabla_funciones[0x06] = JV;
    tabla_funciones[0x07] = JNP;
    tabla_funciones[0x08] = JNN;
    tabla_funciones[0x09] = JNZ;
    tabla_funciones[0x0A] = NOT;
    tabla_funciones[0x0F] = STOP;



}

dword registros[32] = {0};

void inicializar_registros(void){
    for(int i = 0; i < 32; i++)
        registros[i] = 0;

    registros[CS] = 0x00000000;

    registros[DS] = (1 << 16) | 0x0000;

    registros[IP] = registros[CS];

    printf("Procesador inicializado. CS: %08X | DS: %08X | IP %08X\n", registros[CS], registros[DS], registros[IP]);
}

dword leer_valor_operando(byte tipo){
    dword valor = 0;

    switch(tipo){
        case 0x01:{
            valor = leer_memoria_byte(registros[IP]) & 0x1F;
            registros[IP] += 1;
            break;
        }
        case 0x02:{
            byte b1 = leer_memoria_byte(registros[IP]);
            byte b2 = leer_memoria_byte(registros[IP]+1);
            valor  = (b1 << 8) | b2;
            registros[IP] += 2;
            break;
        }
        case 0x03:{
            byte b1 = leer_memoria_byte(registros[IP]);
            byte b2 = leer_memoria_byte(registros[IP]+1);
            byte b3 = leer_memoria_byte(registros[IP]+2);
            valor = (b1 << 16) | (b2 << 8) | (b3 & 0x1F);
            registros[IP] += 3;
            break;
        }


    }

    return valor;
}

void iniciar_ejecucion(void){
    inicializar_tabla_funciones();


    printf("Iniciando ejecucion\n");

    word tamano_codigo = obtenerTamCodigo();
    
    while(registros[IP] != 0xFFFFFFFF && registros[IP] < tamano_codigo){
        byte primer_byte = leer_memoria_byte(registros[IP]);

        byte bits_altos = (primer_byte >> 6) & 0x03;
        byte bits_medios = (primer_byte >> 4) & 0x03;
        byte opc_bajo = primer_byte & 0x0F;

        byte opc, tipo_a, tipo_b;

        if(bits_medios != 0x00){
            // 2 operandos
            tipo_b = bits_altos;
            tipo_a = bits_medios;
            opc = 0x10 | opc_bajo;
        }
        else if(bits_altos != 0x00){
            // 1 operando
            tipo_a = bits_altos;
            tipo_b = 0x00;
            opc = opc_bajo;
        }else{
            // sin operandos
            tipo_a = 0x00;
            tipo_b = 0x00;
            opc = opc_bajo;
        }

        registros[OPC] = opc;

        registros[IP] += 1;



        dword valor_a = 0;
        dword valor_b = 0;

        if(tipo_b != 0x00){
            valor_b = leer_valor_operando(tipo_b);
        }

        if(tipo_a != 0x00){
            valor_a = leer_valor_operando(tipo_a);
        }



        registros[OP1] = (tipo_a << 24) | (valor_a & 0x00FFFFFF);
        registros[OP2] = (tipo_b << 24) | (valor_b & 0x00FFFFFF);


        tabla_funciones[registros[OPC]]();

    }
}