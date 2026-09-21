#define MAXOPERACIONES 32

typedef void (*vecOps[])(void);

extern vecOps vecOperaciones[MAXOPERACIONES];


//instrucciones con 2 operandos
void MOV(void);
void ADD(void);
void SUB(void);
void MUL(void);
void DIV(void);
void CMP(void);
void AND(void);
void OR(void);
void XOR(void);
void SWAP(void);
void SHL(void);
void SHR(void);
void SAR(void);
void LDL(void);
void LDH(void);
void RND(void);

//instrucciones con 1 operando
void SYS(void);
void JMP(void);
void JP(void);
void JN(void);
void JZ(void);
void JC(void);
void JV(void);
void JNP(void);
void JNN(void);
void JNZ(void);
void NOT(void);

//instrucción sin operandos
void STOP(void);






