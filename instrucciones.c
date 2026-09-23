#include "tipos.h"  //contiene los tipos byte, word y dword
#include "cpu.h"    //contiene los nros de los regs
#include "instrucciones.h"
#include <stdio.h>
#include <stdlib.h>     //para RND()
#include <ctype.h>     //para isprint() en SYS()

//instrucciones con 2 operandos

void MOV(void){
    dword b;

    b = leer_valor_operando(registros[OP2]);
    escribir_memoria_dword(registros[OP1],b);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);      //seteo el registro CC en cero

    if ((int32_t)b < 0)
        registros[CC] |= FLAG_N;
    else
        if (b == 0)
            registros[CC] |= FLAG_Z;
}

void ADD(void){
    dword a,b,aux;
    uint64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);
    aux64 = (uint64_t)(int32_t)a + (int32_t)b;
    aux = (dword)aux64;
    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (aux64 >> 32)                    //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b > 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b < 0 && (int32_t)aux > 0))   //dos positivos = neg o dos negativos = positivo
        registros[CC] |= FLAG_V;

}

void SUB(void){
    dword a,b,aux;
    uint64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);
    aux64 = (uint64_t)(int32_t)a-(int32_t)b;
    aux = (dword)aux64;
    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (aux64 >> 32)                //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b < 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b > 0 && (int32_t)aux > 0)   //positivo - negativo = neg o negativo - positivo = positivo
        registros[CC] |= FLAG_V;
}

void MUL(void){
    dword a, b, aux;
    int64_t auxConSigno;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    auxConSigno = (int64_t)(int32_t)a * (int32_t)b;
    aux = (dword)auxConSigno;
    escribir_memoria_dword(registros[OP1], aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (auxConSigno > 2147483647LL || auxConSigno < -2147483648LL) {    // límites de 32 bits
        registros[CC] |= FLAG_C;
        registros[CC] |= FLAG_V;
    }
}

void DIV(void){
    dword a, b, aux,resto;
    int64_t auxConSigno;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    if(b != 0){
        auxConSigno = (int64_t)(int32_t)a / (int32_t)b;
        aux = (dword)auxConSigno;
        escribir_memoria_dword(registros[OP1], aux);

        registros[AC] = (int32_t)a % (int32_t)b;;

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;

        if (auxConSigno > 2147483647LL || auxConSigno < -2147483648LL) {    //límites de 32 bits
            registros[CC] |= FLAG_C;
            registros[CC] |= FLAG_V;
        }
    }
    else{
        printf("\nError: División por cero.");
        registros[IP] = 0xFFFFFFFF;
    }
}

void CMP(void){
    dword a,b,aux;
    uint64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);
    aux64 = (uint64_t)(int32_t)a-(int32_t)b;
    aux = (dword)aux64;

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (aux64 >> 32)                 //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b < 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b > 0 && (int32_t)aux > 0)   //positivo - negativo dieron neg o negativo - positivo dio positivo
        registros[CC] |= FLAG_V;
}

//&, | y ^ hace operaciones columna a columna -> no tienen bit de acarreo ni overflow

void AND(void){
    dword a,b,aux;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux = a & b;
    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

}

void OR(void){
    dword a,b,aux;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux = a | b;
    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;
}

void XOR(void){
    dword a,b,aux;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux = a ^ b;
    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;
}

void SWAP(void){
    dword a,b,aux;
    byte tipoa,tipob;


    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);
    tipoa = registros[OP1] >> 24;
    tipob = (registros[OP2] >> 24;

    if( (tipoa == 1 || tipoa == 3) && (tipob == 1 || tipob == 3){
        a ^= b;
        b ^= a;
        a ^= b;

        escribir_memoria_dword(registros[OP1],a);
        escribir_memoria_dword(registros[OP2],b);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)a < 0)                                     //el último XOR es sobre a
            registros[CC] |= FLAG_N;
        else
            if (a == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: Intento de SWAP con operandos inmediatos");
        registros[IP] = 0xFFFFFFFF;
    }
}

void SHL(void){
    dword a,b,aux;
    uint64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux64 =(uint64_t) a <<  b;    //int de 64 para atrapar el carry
    aux = (dword)aux64;

    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if(aux == 0)
            registros[CC] |= FLAG_Z;

    if (aux64 >> 32)
        registros[CC] |= FLAG_C;

    if ((int32_t)a <0 != (int32_t)aux <0)
        registros[CC] |= FLAG_V;

}

void SHR(void){
    dword a,b,aux;
    uint64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux = a >> b;
    aux64 = ((uint64_t)a<<32) >> b;    //int de 64 para atrapar el carry

    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if(aux == 0)
            registros[CC] |= FLAG_Z;

    if ((dword)aux64 > 0)           //reviso los 32 bits de la parte menos signif.
        registros[CC] |= FLAG_C;

}

void SAR(void){
    dword a,b,aux;
    int64_t aux64;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    aux = (int32_t)a >> b;
    aux64 = (int64_t)((int64_t)a << 32) >> b;    //int de 64 para atrapar el carry

    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if(aux == 0)
            registros[CC] |= FLAG_Z;

    if ((dword)aux64 > 0)           //reviso los 32 bits de la parte menos signif.
        registros[CC] |= FLAG_C;
}

void LDL(void){
    dword a,b,aux;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    a &= 0xFFFF0000;            //apago los bits que se van a reemplazar
    b &= 0x0000FFFF;

    aux = a | b;

    escribir_memoria_dword(registros[OP1],aux);
}

void LDH(void){
    dword a,b,aux;

    a = leer_valor_operando(registros[OP1]);
    b = leer_valor_operando(registros[OP2]);

    a &= 0x0000FFFF;                 //apago los bits que se van a reemplazar
    aux = a | (b << 16);

    escribir_memoria_dword(registros[OP1],aux);
}

void RND(void){     //carga en OP1 un valor entre 0 y b
    dword b,aux;

    b = leer_valor_operando(registros[OP2]);

    if (b != 0xFFFFFFFF)
        aux = rand() % (b+1);    //b+1 para incluir al operando b
    else
        aux = 0;

    escribir_memoria_dword(registros[OP1],aux);
}

//instrucciones con 1 operando

void SYS(void){
    dword a,cantDatos,tamano,aux,modo,posInicial,dirLogica,dirFisica,dato;
    int i,bitsTot;
    char strBin[MAXCADENABINARIA];

    a = leer_valor_operando[OP1];
    aux = registros[ECX];
    modo = registros[EAX];
    posInicial = registros[EDX];

    cantdatos = aux & 0x0000FFFF;
    tamano = (aux >> 16) & 0x0000FFFF;

    if(a == 1)    //lectura
        for(i=0; i < cantDatos; i++){
            dirLogica = posInicial + (tamano*i);
            dirFisica = traducir_direccion(dirLogica,tam);
            printf("\n[%04X]: ",dirFisica);

            switch(modo){
                case 16:scanf("%s",strBin);             //binario
                        dato = strtol(strBin,NULL,2);
                        break;
                case 8: scanf("%x", &dato);        //hexadecimal
                        break;
                case 4: scanf("%o", &dato);            //octal
                        break;
                case 2: scanf("%c", (char*)&dato);           //caracter
                        break;
                case 1: scanf("%d", &dato);                                //decimal
                        break;
                default: printf("\nError: Modo de lectura erróneo.");
                        break;
            }

            if(tam == 1)
                escribir_memoria_byte(dirLogica,(byte)dato);
            else
                if(tam == 4)
                    escribir_memoria_dword(dirLogica,dato);
        }
    else
        if (a == 2){        //escritura

            for(i = 0; i < cantDatos; i++){
                dirLogica = posInicial + (tamano*i);
                dirFisica = traducir_direccion(dirLogica,tam);
                printf("\n[%04X]: ",dirFisica);

                if(tam == 1)
                    leer_memoria_byte(dirLogica,(byte)dato);
                else
                    if(tam == 4)
                        leer_memoria_dword(dirLogica,dato);

                if (modo & 0x01)            // decimal
                    printf("%d ", dato);
                if (modo & 0x02)            // caracter
                    if (isprint(dato))
                        printf("%c ",dato);
                    else
                        printf(". ");
                if (modo & 0x04)            // octal
                    printf("%o ", dato);
                if (modo & 0x08)            // hexadecimal
                    printf("%X ", dato);
                if (modo & 0x10){          // binario
                    bitsTot = tamano * 8;
                    printf("0b");
                    for (i = bitsTot; i > 0; i--)
                        printf("%d", (dato >> i) & 1);
                    printf(" ");
                }

            }
        }
        else
            printf("\nError: Llamada al sistema inválida");
}

void JMP(void){
    dword offset;

    offset = leer_valor_operando(registros[OP1]);
    registros[IP] = offset; //celda: 2B para segmento y 2B para offset
}

//asumo que los guiones en el cuadro ej de la consigna pueden ser 1 o 0, asique apago bits con las máscaras

void JP(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == 0){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JN(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_N){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JZ(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_Z){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JC(void){
    dword offset;

    if((registros[CC] & FLAG_C) == FLAG_C){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JV(void){
    dword offset;

    if((registros[CC] & FLAG_V) == FLAG_V){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNP(void){
    dword offset;

    if(((registros[CC] & FLAG_N) == FLAG_N)||((registros[CC] & FLAG_Z) == FLAG_Z)){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNN(void){
    dword offset;

    if((registros[CC] & FLAG_N) == 0){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNZ(void){
    dword offset;

    if((registros[CC] & FLAG_Z) == 0){
        offset = leer_valor_operando(registros[OP1]);
        registros[IP] = offset;
    }
}

void NOT(void){
    dword a, aux;

    a = leer_valor_operando(registros[OP1]);
    aux = ~a;

    escribir_memoria_dword(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if(aux == 0)
            registros[CC] |= FLAG_Z;
}

//instrucción sin operandos

void STOP(void){
    registros[IP] = 0xFFFFFFFF;
}
