#include "tipos.h"  //contiene los tipos byte, word y dword
#include "cpu/cpu.h" //contiene los nros de los regs
#include "memoria/memoria.h"
#include "instrucciones.h"
#include <stdio.h>
#include <stdlib.h>     //para RND() y clear screen
#include <ctype.h>     //para isprint() en SYS()

//instrucciones con 2 operandos

void MOV(void){
    dword a,b;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        aux = a + b;
        aux64SinSigno = (uint64_t)a + b;
        aux64 = (int64_t)(int32_t)a + (int32_t)b;       //esta forma nos ahorra evaluar casos erroneos para overflow a mano

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
    tipo = registros[OP1] >> TRESBYTES;

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

        if (!(aux64SinSigno >> 32))                //hubo carry
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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

    if (tipo != 2){
        b = getOperando(registros[OP2]);

        if(b != 0){

            if (a == (int32_t)0x80000000 && b == -1) {
            registros[OP1] = 0x80000000;
            registros[AC] = 0;

            // carry / overflow
            registros[CC] |= FLAG_C;
            registros[CC] |= FLAG_V;
            }
            else{
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

                if (aux64 != aux) {    //limites de 32 bits
                    registros[CC] |= FLAG_C;        //-2147483648 / -1 -> hace 2147483648 / 4294967295 que da 0 y deberia dar 2147483648
                    registros[CC] |= FLAG_V;
                }
            }


        }
        else{

            printf("\nError: Division por cero.");
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

    if (!(aux64SinSigno >> 32))                 //hubo carry
        registros[CC] |= FLAG_C;

    if(aux64 != aux)  //positivo - negativo dieron neg o negativo - positivo dio positivo
        registros[CC] |= FLAG_V;
}

//&, | y ^ hace operaciones columna a columna -> no tienen bit de acarreo ni overflow

void AND(void){
    dword a,b,aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
        if ((int32_t)a < 0)                                     //el ultimo XOR es sobre a
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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    tipo = registros[OP1] >> TRESBYTES;

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
    int i,j,bitsTot,empezo,limite,cont;
    char strBin[MAXCADENABINARIA],c,temp;
    byte car;

    a = getOperando(registros[OP1]);

    aux = registros[ECX];
    modo = registros[EAX];
    posInicial = registros[EDX];

    cantDatos = aux & 0x0000FFFF;
    tamano = (aux >> 16) & 0x0000FFFF;

    if (version == 1){
        if(a == 1) {   // lectura
            for(i = 0; i < cantDatos; i++){
                dirLogica = posInicial + (tamano * i);
                dirFisica = traducir_direccion(dirLogica, tamano);
                printf("\n[%04X]: ", dirFisica);

                switch(modo){
                    case 16: scanf("%s", strBin);              // binario
                             dato = strtol(strBin, NULL, 2);
                             break;
                    case 8:  scanf("%x", &dato);               // hexadecimal
                             break;
                    case 4:  scanf("%o", &dato);               // octal
                             break;
                    case 2:  scanf(" %c", (char*)&dato);       // caracter
                             break;
                    case 1:  scanf("%d", &dato);               // decimal
                             break;
                    default: printf("\nError: Modo de lectura erróneo.");
                             dato = 0;
                             break;
                }

                if(tamano == 1)
                    escribir_memoria_byte(dirLogica, (byte)dato);
                else if(tamano == 4)
                    escribir_memoria_dword(dirLogica, dato);
            }
        }
        else if (a == 2) {   // escritura
            for(i = 0; i < cantDatos; i++){
                dirLogica = posInicial + (tamano * i);
                dirFisica = traducir_direccion(dirLogica, tamano);
                printf("\n[%04X]: ", dirFisica);

                if(tamano == 1)
                    dato = leer_memoria_byte(dirLogica);
                else
                    if(tamano == 2){
                        byte b1 = leer_memoria_byte(dirLogica);
                        byte b2 = leer_memoria_byte(dirLogica + 1);
                        dato = (b1 << 8) | b2;
                    }
                    else
                        if(tamano == 4)
                            dato = leer_memoria_dword(dirLogica);

                if (modo & 0x01)            // decimal
                    printf("%d ", dato);

                if (modo & 0x02) {          // caracter
                    if (isprint(dato))
                        printf("%c ", dato);
                    else
                        printf(". ");
                }

                if (modo & 0x04)            // octal
                    printf("%o ", dato);

                if (modo & 0x08)            // hexadecimal
                    printf("%X ", dato);

                if (modo & 0x10) {          // binario
                    printf("0b");
                    if (dato == 0)
                        printf("0");
                    else {
                        empezo = 0;
                        for (j = 31; j >= 0; j--) {
                            if ((dato >> j) & 1)
                                empezo = 1;
                            if (empezo)
                                printf("%d", (dato >> j) & 1);
                        }
                    }
                    printf("\n ");
                }
            }
        }
        else {
            printf("\nError: Llamada al sistema inválida");
            registros[IP] = 0xFFFFFFFF;
            return;
        }
    }
    else
        if(version == 2){
            switch(a) {
                case 0:                             //clear screen
                    system("cls || clear");
                    break;

                case 1:                             //lectura
                    for(i=0; i < cantDatos; i++){
                        dirLogica = posInicial + (tamano*i);
                        dirFisica = traducir_direccion(dirLogica,tamano);
                        printf("\n[%04X]: ",dirFisica);

                        switch(modo){
                            case 16:scanf("%s",strBin);
                                    dato = strtol(strBin,NULL,2);
                                    break;
                            case 8: scanf("%x", &dato);
                                    break;
                            case 4: scanf("%o", &dato);
                                    break;
                            case 2: scanf(" %c", &temp);
                                    dato = temp;
                                    break;
                            case 1: scanf("%d", &dato);
                                    break;
                            default: printf("\nError: Modo de lectura erróneo.");
                                    break;
                        }

                        registros[LAR] = dirLogica;
                        registros[MAR] = (tamano << 16) | (dirFisica & 0xFFFF);
                        registros[MBR] = dato;

                        if(tamano == 1)
                            escribir_memoria_byte(dirLogica,(byte)dato);
                        else
                            if(tamano == 4)
                                escribir_memoria_dword(dirLogica,dato);
                    }
                    break;

                case 2:                                 //escritura
                    for(i = 0; i < cantDatos; i++){
                        dirLogica = posInicial + (tamano*i);
                        dirFisica = traducir_direccion(dirLogica,tamano);
                        printf("\n[%04X]: ",dirFisica);

                        registros[LAR] = dirLogica;
                        registros[MAR] = (tamano << 16) | (dirFisica & 0xFFFF);

                        if(tamano == 1)
                            dato = leer_memoria_byte(dirLogica);
                        else
                            if(tamano == 2) {
                                byte b1 = leer_memoria_byte(dirLogica);
                                byte b2 = leer_memoria_byte(dirLogica + 1);
                                dato = (b1 << 8) | b2;
                            }
                            else
                                if(tamano == 4)
                                    dato = leer_memoria_dword(dirLogica);

                        registros[MBR] = dato;

                        if (modo & 0x01)
                            printf("%d \n", dato);
                        if (modo & 0x02)
                            if (isprint(dato))
                                printf("%c \n",dato);
                            else
                                printf(". \n ");
                        if (modo & 0x04)
                            printf("%o \n", dato);
                        if (modo & 0x08)
                            printf("%X \n", dato);
                        if (modo & 0x10) {          // binario
                            printf("0b");
                            if (dato == 0)
                                printf("0");
                            else {
                                empezo = 0;
                                for (j = 31; j >= 0; j--) {
                                    if ((dato >> j) & 1) empezo = 1;
                                    if (empezo) printf("%d", (dato >> j) & 1);
                                }
                            }
                            printf("\n ");
                        }
                    }
                    break;

                case 3: limite = registros[ECX];                    //STRING READ
                        dirLogica = registros[EDX]; // Puntero inicial
                        cont = 0;

                        while ((c = getchar()) == '\n' || c == EOF);    // Limpio el buffer por si quedó un salto de línea (\n) de ingresos previos

                        while (c != '\n' && c != EOF) {             // Lee hasta que el usuario presione Enter
                            if (limite == -1 || cont < limite) {    // si no hay límite (-1) o si todavía no se pasó
                                escribir_memoria_byte(dirLogica, (byte)c);
                                dirLogica++;
                                cont++;
                            }
                            c = getchar();
                        }
                        escribir_memoria_byte(dirLogica, 0x00); // Cargo el byte nulo '\0' al final del string
                        break;

                case 4: dirLogica = registros[EDX];     // Inicia en la pos apuntada por EDX             //STRING WRITE
                        car = leer_memoria_byte(dirLogica);

                        while (car != 0x00) {
                            printf("%c", car);
                            dirLogica++;
                            car = leer_memoria_byte(dirLogica);
                        }
                        break;

                case 15:if (archivo_vmi_activo)
                            pausa_interactiva = 1;
                        break;

                default:
                    printf("\nError: Llamada al sistema invalida");
                    registros[IP] = 0xFFFFFFFF;
                    return;
                    break;
                }
        }
}

void JMP(void){
    dword offset;

    offset = getOperando(registros[OP1]);
    if (version == 1)
        registros[IP] = offset;
    else
        if (version == 2)
            registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF); //Code Segment en los 16 bits más significativos y offset en los 16 bits menos significativos
}

//asumo que los guiones en el cuadro ej de la consigna pueden ser 1 o 0, asique apago bits con las máscaras

void JP(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == 0){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if (version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JN(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_N){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if (version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JZ(void){
    dword offset;

    if((registros[CC] & (FLAG_N | FLAG_Z)) == FLAG_Z){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if(version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JC(void){
    dword offset;

    if((registros[CC] & FLAG_C) == FLAG_C){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if(version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JV(void){
    dword offset;

    if((registros[CC] & FLAG_V) == FLAG_V){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if(version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JNP(void){
    dword offset;

    if(((registros[CC] & FLAG_N) == FLAG_N)||((registros[CC] & FLAG_Z) == FLAG_Z)){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if(version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JNN(void){
    dword offset;

    if((registros[CC] & FLAG_N) == 0){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if (version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void JNZ(void){
    dword offset;

    if((registros[CC] & FLAG_Z) == 0){
        offset = getOperando(registros[OP1]);
        if (version == 1)
            registros[IP] = offset;
        else
            if (version == 2)
                registros[IP] = (registros[IP] & 0xFFFF0000) | (offset & 0x0000FFFF);
    }
}

void NOT(void){
    dword a, aux;
    byte tipo;

    a = getOperando(registros[OP1]);
    tipo = registros[OP1] >> TRESBYTES;

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


void PUSH(void) {
    dword a;

    if (version == 1) {
        printf("\nError: Instruccion PUSH invalida en version 1");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
    else
        if(version == 2){
            a = getOperando(registros[OP1]);      //getOperando ya devuelve 4B (dword)
            registros[SP] -= 4;
            if (registros[SP] < registros[SS]) {    //verifico stack overflow
                printf("\nError: STACK OVERFLOW. La pila excedió su límite.");
                registros[IP] = 0xFFFFFFFF;
                return;
            }
            else{
                registros[LAR] = registros[SP];
                registros[MBR] = a;
                escribir_memoria_dword(registros[SP], a); //escribir_memoria:dword ya guarda en formato big-endian
            }
        }
}

void POP(void){
    dword a, codSegmento, offset, tamSegmento;

    if (version == 1) {
        printf("\nError: Instruccion POP invalida en version 1");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
    else
        if(version == 2){
            codSegmento = registros[SP] >> 16;      //dirs lógicas: 2B código de segmento y 2B offset
            offset = registros[SP] & 0xFFFF;
            tamSegmento = tabla_segmentos[codSegmento] & 0xFFFF; //tamaño de segmento en los 2B más signif.

            if (offset + 4 > tamSegmento) {
                printf("\nError: STACK UNDERFLOW. La pila está vacía o no tiene bytes suficientes.");
                registros[IP] = 0xFFFFFFFF;
                return;
            }
            else{
                registros[LAR] = registros[SP]; // saco los 4B del tope de la pila, SP es la dir lógica
                a = leer_memoria_dword(registros[SP]);
                registros[MBR] = a;
                setOperando(registros[OP1], a); //paso el valor al operando
                registros[SP] += 4;
            }
        }
}

void CALL(void){    //idem PUSH IP
    dword salto;

    if (version == 1) {
        printf("\nError: Instruccion CALL invalida en version 1");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
    else
        if (version == 2){
            salto = getOperando(registros[OP1]);      //valor de la prox subrutina
            registros[SP] -= 4;
            if (registros[SP] < registros[SS]) {    //verifico stack overflow
                printf("\nError: STACK OVERFLOW. La pila excedió su límite.");
                registros[IP] = 0xFFFFFFFF;
                return;
            }
            else{
                registros[LAR] = registros[SP];
                registros[MBR] = registros[IP];
                escribir_memoria_dword(registros[SP], registros[IP]); //guardo el valor del IP
                registros[IP] = salto;  //JUMP subrutina
            }
        }
}

//instrucción sin operandos

void STOP(void){
    registros[IP] = 0xFFFFFFFF;
}

void RET(void){         //idem POP IP
    dword a, codSegmento, offset, tamSegmento;

    if (version == 1) {
        printf("\nError: Instruccion RET invalida en version 1");
        registros[IP] = 0xFFFFFFFF;
        return;
    }
    else
        if (version == 2){
            codSegmento = registros[SP] >> 16;      //dirs lógicas: 2B código de segmento y 2B offset
            offset = registros[SP] & 0xFFFF;
            tamSegmento = tabla_segmentos[codSegmento] & 0xFFFF; //tamaño de segmento en los 2B más signif.

            if (offset + 4 > tamSegmento) {
                printf("\nError: STACK UNDERFLOW. La pila está vacía o no tiene bytes suficientes.");
                registros[IP] = 0xFFFFFFFF;
                return;
            }
            else{                               //extraigo IP del tope de la pila
                registros[LAR] = registros[SP]; // saco los 4B del tope de la pila, SP es la dir lógica
                registros[IP] = leer_memoria_dword(registros[SP]);
                registros[MBR] = registros[IP];

                registros[SP] += 4;
            }
        }
}

dword getOperando(dword operando) {
    dword tipo, valor, dirLog, aux = 0;
    int32_t valSig;
    byte codReg = 0, tam = 4, sector = 0;

    tipo = operando >> TRESBYTES;
    valor = operando & 0x00FFFFFF; // FFFFFF00

    if(version == 1){
        if (tipo == 2)
            valSig = (int32_t)(valor << 16) >> 16; // Inmediatos (16 bits en la parte baja)
        else
            valSig = (int32_t)(valor << 8) >> 16;  // Memoria (offset de 16 bits en la parte alta)

        if (tipo == 1 || tipo == 3) // Aislo el código de registro
            codReg = valor & 0x1F;

        switch (tipo) {
            case 1: aux = registros[codReg];  // Versión 1: siempre lee 4 bytes completos
                    break;
            case 2:
                aux = valSig; // Valor inmediato
                break;

            case 3:
                dirLog = registros[codReg] + valSig; // Calculo la dirección lógica
                registros[LAR] = dirLog;
                aux = leer_memoria_dword(dirLog); // Versión 1 siempre lee 4B
                break;

            default:
                printf("\nError, tipo de operando no existente.");
                registros[IP] = 0xFFFFFFFF;
                break;
        }
    }
    else
        if(version == 2){
            if (tipo == 2)
                valSig = (int32_t)(valor << 16) >> 16; // Inmediatos (16 bits en la parte baja)
            else
                valSig = (int32_t)(valor << 8) >> 16;  // Memoria (offset de 16 bits en la parte alta)

            if (tipo == 1 || tipo == 3) // Aislo el código de registro
                codReg = valor & 0x1F;

            switch (tipo) {
                case 1: sector = (valor >> 5) & 0x03; //Valor del registro
                        if (sector == 0)
                            aux = registros[codReg];                                 // EAX (4 bytes)
                        else
                            if (sector == 1)
                                aux = (int32_t)(int8_t)(registros[codReg] & 0xFF);       // AL (1 byte con signo)
                            else
                                if (sector == 2)
                                    aux = (int32_t)(int8_t)((registros[codReg] >> 8) & 0xFF);// AH (1 byte con signo)
                                else
                                    if (sector == 3)
                                        aux = (int32_t)(int16_t)(registros[codReg] & 0xFFFF);    // AX (2 bytes con signo)
                        break;
                case 2: aux = valSig;              // Valor inmediato (conservo el signo, ya viene el valor directo)
                        break;
                case 3: tam = (valor >> 5) & 0x07; // Extraemos bits 5 a 7[cite: 14]
                        dirLog = registros[codReg] + valSig;    // Calculo la dirección lógica y leo la RAM

                        registros[LAR] = dirLog;
                        if (tam == 1)          //leo según el tamaño
                            aux = (int32_t)(int8_t)leer_memoria_byte(dirLog); // Extiendo el signo a 4 bytes
                        else
                            if (tam == 2)
                                aux = (int32_t)(int16_t)leer_memoria_word(dirLog); //Extiendo el signo a 4 bytes
                            else
                                if (tam == 4)
                                    aux = leer_memoria_dword(dirLog);
                        break;

                default:printf("\nError, tipo de operando no existente.");
                        registros[IP] = 0xFFFFFFFF;
                        break;
            }
        }
    return aux;
}

void setOperando(dword operando, dword valor) {
    dword tipo, val, dirLog;
    int32_t valSig;
    byte codReg = 0, tam = 4, sector; //tamaño por default de 4

    tipo = operando >> TRESBYTES;
    val = operando & 0x00FFFFFF;

    if (tipo == 1 || tipo == 3)
        codReg = val & 0x1F;

    if (version == 1){
        if (tipo == 1)              // Escritura directa en registro del procesador
            registros[codReg] = valor; //pisa los 4B del reg
        else
            if (tipo == 3) {     // Escritura física en memoria ppal (RAM)
                valSig = (int32_t)(val << 8) >> 16;        // offset
                dirLog = registros[codReg] + valSig;       // La dirección lógica = base + desplazamiento

                tam = (val >> 5) & 0x07;

                registros[LAR] = dirLog;
                registros[MBR] = valor;

                escribir_memoria_dword(dirLog, valor); //versión 1, siempre escribe 4B
            }
        // El tipo inmediato no llega hasta acá, las funciones que almacenan datos en registros[OP1] no hacen nada si hubo un inmediato
    }
    else
        if(version == 2){
            if (tipo == 1){              // Escritura directa en registro del procesador
                sector = (val >> 5) & 0x03;
                switch (sector){
                    case 0: registros[codReg] = valor;  //reg de 4 bytes
                            break;
                    case 1: registros[codReg] = (registros[codReg] & 0xFFFFFF00) | (valor & 0xFF);  //inserto el valor en el byte más bajo
                            break;
                    case 2: registros[codReg] = (registros[codReg] & 0xFFFF00FF) | ((valor & 0xFF) << 8); //inserto el valor en el 3er byte
                            break;
                    case 3: registros[codReg] = (registros[codReg] & 0xFFFF0000) | (valor & 0xFFFF); //reg de 2B
                            break;
                    default: break;
                }
            }
            else
                if (tipo == 3) {     // Escritura física en memoria ppal (RAM)
                    valSig = (int32_t)(val << 8) >> 16;        // offset
                    dirLog = registros[codReg] + valSig;       // La dirección lógica = base + desplazamiento

                    tam = (val >> 5) & 0x07;

                    registros[LAR] = dirLog;
                    registros[MBR] = valor;

                    if (tam == 1)           // Escribe truncando según el tamaño
                        escribir_memoria_byte(dirLog, (byte)valor);
                    else
                        if (tam == 2)
                            escribir_memoria_word(dirLog,(word)valor);
                        else
                            if (tam == 4)
                                escribir_memoria_dword(dirLog, valor);
                }
            // El tipo inmediato no llega hasta acá, las funciones que almacenan datos en registros[OP1] no hacen nada si hubo un inmediato
        }
}
