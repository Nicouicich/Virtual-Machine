//const
#define CantMem 8192
#define CantReg 16
#define Cadenamax 200
#define Nommax 150
#define MaxRotulos 100
#define MaxToken 10
#define OperacionesMax 256
#define PilaDef 200


// Numero de archivos maximos
#define maxArchivos 10

//Memoria
long MemoriaRam[CantMem]={0};

//PUNTERO A FUNCIONES
void (*Funciones[256])(long,long,long,long)={};

//Vector de registros
long Reg[CantReg]={0};


//Numeracion

#define AX 10
#define BX 11
#define CX 12
#define DX 13
#define EX 14
#define FX 15
#define IP 8
#define CC 9
#define DS 2
#define SS 4
#define SP 6
#define BP 7

//STRUCT DE NEMONICOS
typedef struct{
int cantOperandos;
char etiqueta[5];
int codigo;
} Nemonicos;

//Struct de etiquetas
typedef struct {
char Etiqueta[10];
int codigo;
}Labels;


//VECTOR DE REGISTROS DE ARCHIVOS

typedef struct archivos{
char nombre[150];
int abierto;
FILE *arch;
} archivos;

archivos vecArchivos[10];


//VECTOR DE REGISTROS

typedef char Registros [3];

//PROTOTIPOS FUNCIONES
void OpciondeEj(int argc,const char* argv[],char rutaASM[],char rutaIMG[],Registros registros[],Nemonicos vector[],archivos vecArchivos[]);
void buscaArchs(const char* argv[],int argc,archivos vecArchivos[],int operandoResta);
void Lectura(char*,Nemonicos vec[],Registros[],int*);
int hayasterisco(char*);
void Examina(char* ,int *,int *,int*);
void BuscaOperacion(char operacion[],Nemonicos vec[],long *cod,int *CantOperandos, int *error,FILE*,int*);
void creaNemonicos(Nemonicos vec[]);
void lecturaLabels (char*,Labels Rotulos[],int *cantRotulos,int *HayError);
void iniciaRegistro(Registros registros[]);
int esRegistro(char *operando,Registros registros[]);
void tipoOperando(char *operando,Registros registros[],long *tipoOperador);
long DevuelveOperando(long,long ,char[],Labels[],long,int*);
int busquedaLabel(Labels[],char[],int,int*);
long getValor(long operando, long tipoOp);
void seteaCC(long resultado);
void Ejecucion();
long DevuelveInstruccion(long,long,long);
void EscribeBinario(char*);
void CargaImagen(char*);
long OperandoIndirecto(long );

//PROTOTIPOS FUNCIONES DEL MICRO
void MOV(long op1,long tipoOp1,long op2,long tipoOp2);
void ADD(long op1,long tipoOp1,long op2,long tipoOp2);
void SUB(long op1,long tipoOp1,long op2,long tipoOp2);
void MUL(long op1,long tipoOp1,long op2,long tipoOp2);
void DIV(long op1,long tipoOp1,long op2,long tipoOp2);
void MOD(long op1,long tipoOp1,long op2,long tipoOp2);
void CMP(long op1,long tipoOp1,long op2,long tipoOp2);
void SWAP(long op1,long tipoOp1,long op2,long tipoOp2);
void RND(long op1,long tipoOp1,long op2,long tipoOp2);
void AND(long op1,long tipoOp1,long op2,long tipoOp2);
void OR(long op1,long tipoOp1,long op2,long tipoOp2);
void NOT(long op1,long tipoOp1,long op2,long tipoOp2);
void XOR(long op1,long tipoOp1,long op2,long tipoOp2);
void SHL(long op1,long tipoOp1,long op2,long tipoOp2);
void SHR(long op1,long tipoOp1,long op2,long tipoOp2);
void JMP(long op1,long tipoOp1,long op2,long tipoOp2);
void JE(long op1,long tipoOp1,long op2,long tipoOp2);
void JG(long op1,long tipoOp1,long op2,long tipoOp2);
void JL(long op1,long tipoOp1,long op2,long tipoOp2);
void JZ(long op1,long tipoOp1,long op2,long tipoOp2);
void JP(long op1,long tipoOp1,long op2,long tipoOp2);
void JN(long op1,long tipoOp1,long op2,long tipoOp2);
void JNZ(long op1,long tipoOp1,long op2,long tipoOp2);
void JNP(long op1,long tipoOp1,long op2,long tipoOp2);
void JNN(long op1,long tipoOp1,long op2,long tipoOp2);
void SYS(long op1,long tipoOp1,long op2,long tipoOp2);
void STOP(long op1,long tipoOp1,long op2,long tipoOp2);
void PUSH(long op1,long tipoOp1,long op2,long tipoOp2);
void POP(long op1,long tipoOp1,long op2,long tipoOp2);
void PUSHALL(long op1,long tipoOp1,long op2,long tipoOp2);
void POPALL(long op1,long tipoOp1,long op2,long tipoOp2);
void CALL(long op1,long tipoOp1,long op2,long tipoOp2);
void SMOV(long op1,long tipoOp1,long op2,long tipoOp2);
void SLEN(long op1,long tipoOp1,long op2,long tipoOp2);
void SCMP(long op1,long tipoOp1,long op2,long tipoOp2);


