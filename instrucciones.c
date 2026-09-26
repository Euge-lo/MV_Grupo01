#include "tipos.h"  //contiene los tipos byte, word y dword
#include "cpu/cpu.h" //contiene los nros de los regs
#include "memoria/memoria.h"
#include "instrucciones.h"
#include <stdio.h>
#include <stdlib.h>     //para RND()
#include <ctype.h>     //para isprint() en SYS()

//instrucciones con 2 operandos

void MOV(void){
    dword a,b;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);
        setOperando(registros[OP1],b);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);      //seteo el registro CC en cero

        if ((int32_t)b < 0)
            registros[CC] |= FLAG_N;
        else
            if (b == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void ADD(void){
    dword a,b,aux;
    uint64_t aux64SinSigno;
    int64_t aux64;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a + b;
        aux64SinSigno = (uint64_t)a + b;
        aux64 = (int64_t)(int32_t)a + (int32_t)b;       //esta forma nos ahorra evaluar casos erróneos para overflow a mano

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;

        if (aux64SinSigno >> 32 & 0x00000000FFFFFFFF)                    //hubo carry
            registros[CC] |= FLAG_C;

        if(aux != aux64)            //dos positivos = neg o dos negativos = positivo
            registros[CC] |= FLAG_V;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void SUB(void){
    dword a,b,aux;
    uint64_t aux64SinSigno;
    int64_t aux64;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a - b;
        aux64SinSigno = (uint64_t)a - b;
        aux64 = (int64_t)(int32_t)a - (int32_t)b;

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;

        if (aux64SinSigno >> 32 & 0x00000000FFFFFFFF)                //hubo carry
            registros[CC] |= FLAG_C;

        if(aux != aux64)   //positivo - negativo = neg o negativo - positivo = positivo
            registros[CC] |= FLAG_V;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }

}

void MUL(void){
    dword a, b, aux;
    int64_t aux64;
    uint64_t aux64SinSigno;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a * b;
        aux64SinSigno = (uint64_t)a * b;
        aux64 = (int64_t)(int32_t)a * (int32_t)b;

        setOperando(registros[OP1], aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;

        if (aux64SinSigno >> 32 & 0x00000000FFFFFFFF)
            registros[CC] |= FLAG_C;

        if (aux != aux64)     // límites de 32 bits
            registros[CC] |= FLAG_V;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void DIV(void){
    dword a,b,aux,resto;
    int64_t aux64;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        if(b != 0){
            aux = a / b;
            aux64 = (int64_t)(int32_t)a / (int32_t)b;

            setOperando(registros[OP1], aux);

            registros[AC] = (int32_t)a % (int32_t)b;
            registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

            if ((int32_t)aux < 0)
                registros[CC] |= FLAG_N;
            else
                if (aux == 0)
                    registros[CC] |= FLAG_Z;

            if (aux64 != aux) {    //límites de 32 bits
                registros[CC] |= FLAG_C;        //-2147483648 / -1 -> hace 2147483648 / 4294967295 que da 0 y debería dar 2147483648
                registros[CC] |= FLAG_V;
            }
        }
        else{
            printf("\nError: División por cero.");
            registros[IP] = 0xFFFFFFFF;
            return;
        }
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

//En división de enteros n bits, el único caso que produce overflow es (-2ⁿ⁻¹) ÷ (-1) pq es el único par donde cociente (2ⁿ⁻¹) > 2ⁿ⁻¹ − 1.

void CMP(void){
    dword a,b,aux;
    uint64_t aux64SinSigno;
    int64_t aux64;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);

    aux = a - b;
    aux64SinSigno = (uint64_t)a - b;
    aux64 = (uint64_t)(int32_t)a -(int32_t)b;

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (aux64SinSigno >> 32 & 0x00000000FFFFFFFF)                 //hubo carry
        registros[CC] |= FLAG_C;

    if(aux64 != aux)  //positivo - negativo dieron neg o negativo - positivo dio positivo
        registros[CC] |= FLAG_V;
}

//&, | y ^ hace operaciones columna a columna -> no tienen bit de acarreo ni overflow

void AND(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a & b;
        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void OR(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a | b;
        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void XOR(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a ^ b;
        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void SWAP(void){
    dword a,b,aux;
    byte tipoa,tipob;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);
    tipoa = registros[OP1] >> 24;
    tipob = registros[OP2] >> 24;

    if(tipoa!= 2 && tipob != 2){
        a ^= b;
        b ^= a;
        a ^= b;

        setOperando(registros[OP1],a);
        setOperando(registros[OP2],b);

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
        return;
    }
}

void SHL(void){
    dword a,b,aux;
    uint64_t aux64SinSigno;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a << b;
        aux64SinSigno =(uint64_t) a <<  b;    //unsigned int de 64 para atrapar el carry

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if(aux == 0)
                registros[CC] |= FLAG_Z;

        if (aux64SinSigno >> 32 & 0x00000000FFFFFFFF)
            registros[CC] |= FLAG_C;

        if ((int32_t)a <0 != (int32_t)aux <0) //positivo se vuelve neg o viceversa
            registros[CC] |= FLAG_V;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void SHR(void){
    dword a,b,aux;
    uint64_t aux64SinSigno;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a >> b;
        aux64SinSigno = ((uint64_t)a << 32) >> b;    //unsigned int de 64 para atrapar el carry

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if(aux == 0)
                registros[CC] |= FLAG_Z;

        if ((aux64SinSigno & 0x00000000FFFFFFFF) > 0)           //reviso los 32 bits de la parte menos signif.
            registros[CC] |= FLAG_C;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }

}

void SAR(void){
    dword a,b,aux;
    int64_t aux64;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = (int32_t)a >> b;
        aux64 = (int64_t)((int64_t)a << 32) >> b;    //int de 64 para atrapar el carry

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if(aux == 0)
                registros[CC] |= FLAG_Z;

        if ((aux64 & 0x00000000FFFFFFFF) > 0)           //reviso los 32 bits de la parte menos signif.
            registros[CC] |= FLAG_C;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void LDL(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        a &= 0xFFFF0000;            //apago los bits que se van a reemplazar
        b &= 0x0000FFFF;

        aux = a | b;

        setOperando(registros[OP1],aux);
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void LDH(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        a &= 0x0000FFFF;                 //apago los bits que se van a reemplazar
        aux = a | (b << 16);

        setOperando(registros[OP1],aux);
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

void RND(void){     //carga en OP1 un valor entre 0 y b
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        if (b != 0xFFFFFFFF)
            aux = rand() % (b+1);    //b+1 para incluir al operando b
        else
            aux = 0;

        setOperando(registros[OP1],aux);
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

//instrucciones con 1 operando

void SYS(void){
    dword a,cantDatos,tamano,aux,modo,posInicial,dirLogica,dirFisica,dato;
    int i,bitsTot;
    char strBin[MAXCADENABINARIA];

    a = getOperando(registros[OP1]);
    aux = registros[ECX];
    modo = registros[EAX];
    posInicial = registros[EDX];

    cantDatos = aux & 0x0000FFFF;
    tamano = (aux >> 16) & 0x0000FFFF;

    if(a == 1)    //lectura
        for(i=0; i < cantDatos; i++){
            dirLogica = posInicial + (tamano*i);
            dirFisica = traducir_direccion(dirLogica,tamano);
            printf("\n[%04X]: ",dirFisica);

            switch(modo){
                case 16:scanf("%s",strBin);             //binario
                        dato = strtol(strBin,NULL,2);
                        break;
                case 8: scanf("%x", &dato);        //hexadecimal
                        break;
                case 4: scanf("%o", &dato);            //octal
                        break;
                case 2: scanf(" %c", (char*)&dato);           //caracter
                        break;
                case 1: scanf("%d", &dato);                                //decimal
                        break;
                default: printf("\nError: Modo de lectura erróneo.");
                        break;
            }

            if(tamano == 1)
                escribir_memoria_byte(dirLogica,(byte)dato);
            else
                if(tamano == 4)
                    escribir_memoria_dword(dirLogica,dato);
        }
    else
        if (a == 2){         //escritura

            for(i = 0; i < cantDatos; i++){
                dirLogica = posInicial + (tamano*i);
                dirFisica = traducir_direccion(dirLogica,tamano);
                printf("\n[%04X]: ",dirFisica);

                if(tamano == 1)
                    dato = leer_memoria_byte(dirLogica);
                else
                    if(tamano == 4)
                       dato = leer_memoria_dword(dirLogica);

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
                    for (i = bitsTot; i >= 0; i--)
                        printf("%d", (dato >> i) & 1);
                    printf(" ");
                }

            }
        }
        else{
            printf("\nError: Llamada al sistema inválida");
            registros[IP] = 0xFFFFFFFF;
            return;
        }
}

void JMP(void){
    dword offset;

    offset = getOperando(registros[OP1]);
    registros[IP] = offset; //celda: 2B para segmento y 2B para offset
}

//asumo que los guiones en el cuadro ej de la consigna pueden ser 1 o 0, asique apago bits con las máscaras

void JP(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == 0){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JN(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_N){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JZ(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_Z){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JC(void){
    dword offset;

    if((registros[CC] & FLAG_C) == FLAG_C){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JV(void){
    dword offset;

    if((registros[CC] & FLAG_V) == FLAG_V){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNP(void){
    dword offset;

    if(((registros[CC] & FLAG_N) == FLAG_N)||((registros[CC] & FLAG_Z) == FLAG_Z)){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNN(void){
    dword offset;

    if((registros[CC] & FLAG_N) == 0){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void JNZ(void){
    dword offset;

    if((registros[CC] & FLAG_Z) == 0){
        offset = getOperando(registros[OP1]);
        registros[IP] = offset;
    }
}

void NOT(void){
    dword a, aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = a >> 3BYTES;

    if (tipo != 2){
        aux = ~a;

        setOperando(registros[OP1],aux);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if(aux == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: El operando de destino es de tipo inmediato");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
}

//instrucción sin operandos

void STOP(void){
    registros[IP] = 0xFFFFFFFF;
}

dword getOperando(dword op) {
    dword tipo, valor, dirLog, aux;
    int32_t valSig;
    byte codReg = 0;

    tipo = op >> 3BYTES;
    valor = op & 0x00FFFFFF; //FFFFFF00
    valSig = (int32_t)(valor << 8) >> 16; //por si es valor inmediato u offset del tipo memoria

    if (tipo == 1 || tipo == 3) //aislo el código de registro
        codReg = valor & 0x1F;

    switch (tipo) {
        case 1: aux = registros[codReg];   //valor del registro
                break;
        case 2: aux = valSig;              //valor inmediato (conservo el signo, ya viene el valor directo)
                break;
        case 3: dirLog = registros[codReg] + valSig;    // Calculo la dirección lógica y leo la RAM
                aux = leer_memoria_dword(dirLog);
                break;

        default:print("\nError, tipo de operando no existente.");
                registros[IP] = 0xFFFFFFFF;
                break;
    }

    return aux;
}

void setOperando(dword operando, dword valor) {
    dword tipo, val, dirLog;
    int32_t valSig;
    byte codReg = 0;

    tipo = operando >> 3BYTES;
    val = operando & 0x00FFFFFF;

    if (tipo == 1 || tipo == 3)
        codReg = val & 0x1F;

    if (tipo == 1)              // Escritura directa en registro del procesador
        registros[codReg] = valor;
    else
        if (tipo == 3) {     // Escritura física en memoria (RAM)
            valSig = (int32_t)(val << 8) >> 16;        //offset
            dirLog = registros[codReg] + valSig;      // La dirección lógica = base + desplazamiento

            registros[LAR] = dirLog;         // Actualizamos los registros
            registros[MBR] = valor;
            escribir_memoria_dword(dirLog, valor);
        }
    // El tipo inmediato, no llega hasta acá, las funciones que almacenan datos en registros[OP1] no hacen nada si hubo un inmediato
}


