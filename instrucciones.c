#include "tipos.h"  //contiene los tipos byte, word y dword
#include "cpu.h"    //contiene los nros de los regs
#include "instrucciones.h"

//instrucciones con 2 operandos

void MOV(void){ //asigna B en el primer operando de la instrucción, actualiza CC
    dword b;

    b = getOperando(registros[OP2]);
    setOperando(registros[OP1],b);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);      //seteo el registro CC en cero

    if ((int32_t)b < 0)             // B negativo 0 cero
        registros[CC] |= FLAG_N;
    else
        if (b == 0)
            registros[CC] |= FLAG_Z;
}

void ADD(void){
    dword a,b,aux;
    uint64_t auxConSigno;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);
    auxConSigno = (uint64_t)(int32_t)a + (int32_t)b;
    aux = (dword)auxConSigno;
    setOperando(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)             // suma negativa
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (auxConSigno >> 32)          //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b > 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b < 0 && (int32_t)aux > 0))   //dos positivos dieron neg o viceversa
        registros[CC] |= FLAG_V;

}

void SUB(void){
    dword a,b,aux;
    uint64_t auxConSigno;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);
    auxConSigno = (uint64_t)(int32_t)a-(int32_t)b;
    aux = (dword)auxConSigno;
    setOperando(registros[OP1],aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)             // resta negativa
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (auxConSigno >> 32)          //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b < 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b > 0 && (int32_t)aux > 0)   //positivo - negativo dieron neg o negativo - positivo dio positivo
        registros[CC] |= FLAG_V;
}

void MUL(void){
    dword a, b, aux;
    int64_t auxConSigno;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);

    auxConSigno = (int64_t)(int32_t)a * (int32_t)b;
    aux = (dword)auxConSigno;
    setOperando(registros[OP1], aux);

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

    if ((int32_t)aux < 0)
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (auxConSigno > 2147483647LL || auxConSigno < -2147483648LL) {    // Usó más de 32 bits y el resultado está mal
        registros[CC] |= FLAG_C;
        registros[CC] |= FLAG_V;
    }
}

void DIV(void){
    dword a, b, aux,resto;
    int64_t auxConSigno;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);

    if(b != 0){
        auxConSigno = (int64_t)(int32_t)a / (int32_t)b;
        aux = (dword)auxConSigno;
        setOperando(registros[OP1], aux);

        registros[AC] = (int32_t)a % (int32_t)b;;

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);

        if ((int32_t)aux < 0)
            registros[CC] |= FLAG_N;
        else
            if (aux == 0)
                registros[CC] |= FLAG_Z;

        if (auxConSigno > 2147483647LL || auxConSigno < -2147483648LL) {    // Usó más de 32 bits y el resultado está mal
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
    uint64_t auxConSigno;

    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);
    auxConSigno = (uint64_t)(int32_t)a-(int32_t)b;
    aux = (dword)auxConSigno;

    registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
    if ((int32_t)aux < 0)             // resta negativa
        registros[CC] |= FLAG_N;
    else
        if (aux == 0)
            registros[CC] |= FLAG_Z;

    if (auxConSigno >> 32)          //hubo carry
        registros[CC] |= FLAG_C;

    if(((int32_t)a > 0 && (int32_t)b < 0 && (int32_t)aux < 0) || ((int32_t)a < 0 && (int32_t)b > 0 && (int32_t)aux > 0)   //positivo - negativo dieron neg o negativo - positivo dio positivo
        registros[CC] |= FLAG_V;
}

//&, | y ^ hace operaciones columna a columna -> no tienen bit de acarreo ni overflow

void AND(void){
    dword a,b,aux;

    a = getOperando(registros[OP1]);
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

void OR(void){
    dword a,b,aux;

    a = getOperando(registros[OP1]);
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

void XOR(void){
    dword a,b,aux;

    a = getOperando(registros[OP1]);
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

void SWAP(void){
    dword a,b,aux;
    byte tipoa,tipob;


    a = getOperando(registros[OP1]);
    b = getOperando(registros[OP2]);
    tipoa = registros[OP1] >> 24;
    tipob = (registros[OP2] >> 24;

    if( (tipoa == 1 || tipoa == 3) && (tipob == 1 || tipob == 3){
        a ^= b;
        b ^= a;
        a ^= b;

        setOperando(registros[OP1],a);
        setOperando(registros[OP2],b);

        registros[CC] &= ~(FLAG_N | FLAG_Z | FLAG_C | FLAG_V);
        if ((int32_t)a < 0)                 //el último XOR es sobre a
            registros[CC] |= FLAG_N;
        else
            if (a == 0)
                registros[CC] |= FLAG_Z;
    }
    else{
        printf("\nError: Intento de SWAP con operandos inmediatos.");
        registros[IP] = 0xFFFFFFFF;
    }
}

void SHL(void){

}

void SHR(void){

}

void SAR(void){

}

void LDL(void){

}

void LDH(void){

}

void RND(void){

}

//instrucciones con 1 operando

void SYS(void){

}

void JMP(void){

}

void JP(void){

}

void JN(void){

}

void JZ(void){

}

void JC(void){

}

void JV(void){

}

void JNP(void){

}

void JNN(void){

}

void JNZ(void){

}

void NOT(void){

}

//instrucción sin operandos

void STOP(void){

}
