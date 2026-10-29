#include <stdio.h>
#include <string.h>
#include "dissasembler.h"
#include "memoria/memoria.h"
#include "cpu/cpu.h"

const char* mnemonicos[32] = {
  "SYS", "JMP", "JP", "JN", "JZ", "JC", "JV", "JNP", "JNN", "JNZ", "NOT", "!!!", "!!!", "!!!", "!!!", "STOP",
  "MOV", "ADD", "SUB", "MUL", "DIV", "CMP", "AND", "OR", "XOR", "SWAP", "SHL", "SHR", "SAR", "LDL", "LDH", "RND"
};

const char* nom_registros[32] = {"IP", "OPC", "OP1", "OP2", "LAR", "MAR", "MBR", "SP", "BP", "RES", "EAX", "EBX", "ECX", "EDX", "EEX", "EFX", "AC",
    "CC", "RES", "RES", "RES", "RES", "RES", "RES", "RES", "RES", "CS", "DS", "ES", "SS", "KS", "PS",
};

int arma_dissa(byte tipo, dword ip_dissa, char* str_op, char* str_hex){
    switch(tipo){
        case 0x01: {// registro
            byte b1 = leer_memoria_byte(ip_dissa);
            byte reg = b1 & 0x1F;
            byte sectorRegistro = (b1 >> 5) & 0x03;

            char nombre_reg[5];
            strcpy(nombre_reg, nom_registros[reg]);

            if(reg >= 10 && reg <= 15 && version == 2){
                if(sectorRegistro == 1){
                    nombre_reg[0] = nombre_reg[1];
                    nombre_reg[1] = 'L';
                    nombre_reg[2] = '\0';
                }else if(sectorRegistro == 2){
                    nombre_reg[0] = nombre_reg[1];
                    nombre_reg[1] = 'H';
                    nombre_reg[2] = '\0';
                }else if(sectorRegistro == 3){
                    nombre_reg[0] = nombre_reg[1];
                    nombre_reg[1] = 'X';
                    nombre_reg[2] = '\0';
                }
            }

            sprintf(str_op, "%s", nombre_reg);
            sprintf(str_hex + strlen(str_hex), " %02X", b1);//usamos strlen para que no pise el valor que ya teniamos y lo agregue despues de el texto que ya hay

            return 1; //retornamos la cantidad de bytes que leimos
        }
        case 0x02: {// inmediato
            byte b1 = leer_memoria_byte(ip_dissa);
            byte b2 = leer_memoria_byte(ip_dissa+1);

            short valor = (short)((b1 << 8) | b2);

            sprintf(str_op, "0x%X", (unsigned short)valor);

            sprintf(str_hex + strlen(str_hex), " %02X %02X", b1, b2);

            return 2;
        }
        case 0x03: {// memoria
            byte b1 = leer_memoria_byte(ip_dissa);
            byte b2 = leer_memoria_byte(ip_dissa+1);
            byte b3 = leer_memoria_byte(ip_dissa+2);


            sprintf(str_hex + strlen(str_hex), " %02X  %02X  %02X", b1, b2, b3);

            short offset = (short)((b1 << 8) | b2);
            byte reg = b3 & 0x1F;
            byte tamMem = (b1 >> 5) & 0x07;

            char prefijo[2] = "";
            if(version == 2){
                if (tamMem == 1) {
                    strcpy(prefijo, "b");
                } else if (tamMem == 2) {
                    strcpy(prefijo, "w");
                } else if (tamMem == 4) {
                    strcpy(prefijo, "l");
                }
            }


            if(offset == 0){
                sprintf(str_op, "%s[%s]", prefijo, nom_registros[reg]);
            }else if(offset > 0){
                sprintf(str_op, "%s[%s+%d]", prefijo, nom_registros[reg], offset);
            }else{
                sprintf(str_op, "%s[%s%d]", prefijo, nom_registros[reg], offset);
            }

            return 3;
        }
        default:
            return 0;
    }
}


dword desensamblar(dword ip_actual, dword entry_point){
    char str_hex[32] = "";
    char str_opa[32] = "";
    char str_opb[32] = "";

    dword ip_dissa = ip_actual;


    byte primer_byte = leer_memoria_byte(ip_dissa);
    sprintf(str_hex, " %02X", primer_byte);
    ip_dissa += 1;

    byte bits_opc = primer_byte & 0x0F;
    byte bits_alto = (primer_byte >> 6) & 0x03;
    byte bits_medio = (primer_byte >> 4) & 0x03;

    byte tipo_a = 0x00, tipo_b = 0x00, opc = 0x00;

    if(bits_medio != 0x00){
        tipo_a = bits_medio;
        tipo_b = bits_alto;
        opc = 0x10 | bits_opc;
    }else if(bits_alto != 0x00){
        tipo_a = bits_alto;
        opc = bits_opc;
    }else{
        opc = bits_opc;
    }

    if(tipo_b != 0x00) {
        ip_dissa += arma_dissa(tipo_b, ip_dissa, str_opb, str_hex);
    }

    if(tipo_a != 0x00){
        ip_dissa += arma_dissa(tipo_a, ip_dissa, str_opa, str_hex);
    }

    char marcador[3] = "  ";
    if(version == 2){
        
        if(ip_actual == entry_point){
            strcpy(marcador, "> ");
        }
    }else{
        strcpy(marcador, "");
    }


    if (tipo_b != 0x00) {
        printf("%s[%04X] %-21s |  %-7s  %s, %s\n", marcador, ip_actual, str_hex, mnemonicos[opc], str_opa, str_opb);
    } else if (tipo_a != 0x00) {
        printf("%s[%04X] %-21s |  %-7s  %s\n", marcador, ip_actual, str_hex, mnemonicos[opc], str_opa);
    } else {
        printf("%s[%04X] %-21s |  %-7s\n", marcador, ip_actual, str_hex, mnemonicos[opc]);
    }


    return ip_dissa;
}


void desensamblar_constantes(dword direccion_inicio, dword tamano){
    dword ip_actual = direccion_inicio;

    while(ip_actual < direccion_inicio + tamano){
        char str_hex[30] = "";
        char str_texto[256] = "";
        int i = 0;
        byte caracter;

        dword direccion_string = ip_actual;

        do{
            caracter = leer_memoria_byte(ip_actual);

            if(i <= 6){
                sprintf(str_hex + strlen(str_hex), "%02X ", caracter);
            }
            if(caracter == 0){

            }
            else if(isprint(caracter)){
                sprintf(str_texto + strlen(str_texto), "%c", caracter);
            }else{
                strcat(str_texto, ".");
            }

            ip_actual++;
            i++;
        }while(caracter != 0);

        if(i > 7){
            str_hex[18] = '.';
            str_hex[19] = '.';
            str_hex[20] = '\0';
        }

        printf(" [%04X] %-21s | \"%s\"\n", direccion_string, str_hex, str_texto);
    }
}