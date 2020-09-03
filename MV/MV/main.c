#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>
#include "Prototipos.h"


// ////////

void iniciaRegistro(Registros registros[]) {
    strcpy(registros[AX], "AX");
    strcpy(registros[BX], "BX");
    strcpy(registros[CX], "CX");
    strcpy(registros[DX], "DX");
    strcpy(registros[EX], "EX");
    strcpy(registros[FX], "FX");
    strcpy(registros[DS], "DS");
    strcpy(registros[IP], "IP");
    strcpy(registros[CC], "CC");
    strcpy(registros[SS], "SS");
    strcpy(registros[SP], "SP");
    strcpy(registros[BP], "BP");
    Reg[AX]=0;
    Reg[BX]=0;
    Reg[CX]=0;
    Reg[DX]=0;
    Reg[EX]=0;
    Reg[FX]=0;
    Reg[IP]=0;
    Reg[CC]=0;
    Reg[DS]=0;
    Reg[SS]=0;
    Reg[SP]=0;
    Reg[BP]=0;
}

int esRegistro(char *operando,Registros registros[]){

int i=0;
*operando= toupper(*operando);
while (i<CantReg && strcmp(registros[i],operando)!=0)
    i++;

if (i<CantReg)
    return i;
else
    return -1;

}


void tipoOperando(char *operando,Registros registros[],long *tipoOperador){

    int Registro;

    Registro = esRegistro(operando, registros);

    if (Registro != -1) //ES UN REGISTRO
        *tipoOperador = 1;
    else        //NO ES REGISTRO
            if (operando[0]=='['){
                if((operando[1]>='a' && operando[1]<='z') || (operando[1]>='A' && operando[1]<='Z')){
                    *tipoOperador = 3; // ES DIRECTO
                }
                else{
                    *tipoOperador = 2; // INDIRECTO
                }
            }
            else{
                if (operando[0] == '#' || operando[0] == '@' || operando[0] == '%' || operando[0]>0 ) //ES INMEDIATO //al ser mayor q 0 incluye todos los posibles errores, hay q buscar otra forma
                    *tipoOperador = 0;
                else
                    *tipoOperador = 4; // Registro invalido
            }


}


long getValor(long operando, long tipoOp){
long aux;

switch (tipoOp){
    case 0 : aux=operando; break;
    case 1: aux = Reg[operando]; break;
    case 2: aux = MemoriaRam[Reg[DS] + operando]; break;
    case 3: aux=MemoriaRam[OperandoIndirecto(operando)];break;

    default: ;
}

 return aux;
}

void seteaCC(long resultado){

if (resultado==0)
        Reg[CC] = Reg[CC] | 0x00000001; //SETEA ULTIMO BIT EN 1
    else
        Reg[CC] = Reg[CC] & 0xFFFFFFFE; //SETEA ULTIMO BIT EN 0

    if (resultado<0)
        Reg[CC] = Reg[CC] | 0x80000000; //SETEA PRIMER BIT EN 1
    else
        Reg[CC] = Reg[CC] & 0x7FFFFFFF; //SETEA PRIMER BIT EN 0

}

long OperandoIndirecto(long op){
    long ret=0,reg,cte;

    reg= op & 0xFF;
    cte=(op>>8);
    if((reg==SP) || (reg==BP)){
        ret=Reg[SS];
    }
    else{
        ret=Reg[DS];
    }
    return ret+Reg[reg]+cte;
}



void MOV(long op1,long tipoOp1,long op2,long tipoOp2){
long valor =getValor(op2,tipoOp2);


    if (tipoOp1==1)
        Reg[op1]=valor;
    else
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor;
        }
}

void ADD (long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op2,tipoOp2),resultado,aux;

    if (tipoOp1==1){
        Reg[op1]+=valor;
        resultado=Reg[op1];
    }
    else{
        if(tipoOp1==3){
            aux=OperandoIndirecto(op1);
            MemoriaRam[aux]+=valor;
            resultado=MemoriaRam[aux];
        }
        else{
            MemoriaRam[Reg[DS] + op1]+=valor;
            resultado=MemoriaRam[Reg[DS] + op1];
        }
    }
    seteaCC(resultado);
}

void SUB(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op2,tipoOp2),resultado,aux;

    if (tipoOp1==1){
        Reg[op1]-=valor;
        resultado=Reg[op1];
    }
    else{
        if(tipoOp1==3){
            aux=OperandoIndirecto(op1);
            MemoriaRam[aux]-=valor;
            resultado=MemoriaRam[aux];
        }
        else{
            MemoriaRam[Reg[DS] + op1]-=valor;
            resultado=MemoriaRam[Reg[DS] + op1];
        }
    }
    seteaCC(resultado);
}

void MUL(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op2,tipoOp2),resultado,aux;

    if (tipoOp1==1){
        Reg[op1]*=valor;
        resultado=Reg[op1];
    }
    else{
        if(tipoOp1==3){
            aux=OperandoIndirecto(op1);
            MemoriaRam[aux]*=valor;
            resultado=MemoriaRam[aux];
        }
        else{
            MemoriaRam[Reg[DS] + op1]*=valor;
            resultado=MemoriaRam[Reg[DS] + op1];
        }
    }
    seteaCC(resultado);
}

void DIV(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op2,tipoOp2),resultado,aux;

    if (valor!=0){
        if (tipoOp1==1){
            Reg[op1]/=valor;
            resultado=Reg[op1];
        }
        else{
            if(tipoOp1==3){
                aux=OperandoIndirecto(op1);
                MemoriaRam[aux]/=valor;
                resultado=MemoriaRam[aux];
            }
            else{
                MemoriaRam[Reg[DS] + op1]/=valor;
                resultado=MemoriaRam[Reg[DS] + op1];
            }
        }
        seteaCC(resultado);
    }
    else{
        printf("NO SE PUEDE DIVIDIR POR 0");
    }
}

void MOD(long op1,long tipoOp1,long op2,long tipoOp2){

    long valor = getValor(op2,tipoOp2),resultado,aux;

    if (tipoOp1==1){
        Reg[op1]%=valor;
        resultado=Reg[op1];
        if (Reg[op1]<0)
            Reg[op1]*=-1;
    }
    else{
        if(tipoOp1==3){
            aux=OperandoIndirecto(op1);
            MemoriaRam[aux]%=valor;
            resultado=MemoriaRam[aux];
        }
        else{
            MemoriaRam[Reg[DS] + op1]%=valor;
            resultado=MemoriaRam[Reg[DS] + op1];

            if (MemoriaRam[Reg[DS] + op1]<0)
                MemoriaRam[Reg[DS] + op1]*=-1;
        }
    }


    seteaCC(resultado);
}

void CMP(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op2,tipoOp2),resultado;

    if (tipoOp1==1)
        resultado=Reg[op1]-valor;
    else{
        if(tipoOp1==3){
            resultado=MemoriaRam[OperandoIndirecto(op1)]-valor;
        }
        else{
            resultado=MemoriaRam[Reg[DS] + op1]-valor;
        }
    }

    seteaCC(resultado);
}

void SWAP(long op1,long tipoOp1,long op2,long tipoOp2){

long valor1 = getValor(op1,tipoOp1);
long valor2 = getValor(op2,tipoOp2);

    if (tipoOp1==1)
        Reg[op1]=valor2;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor2;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor2;
        }
    }

    if (tipoOp2==1)
        Reg[op2]=valor1;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op2)]=valor1;
        }
        else{
            MemoriaRam[Reg[DS] + op2]=valor1;
        }
    }
}

void RND(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op2,tipoOp2);
    long aleatorio =rand() %valor ;

    if (tipoOp1==1)
        Reg[op1]=aleatorio;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=aleatorio;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=aleatorio;
        }
    }
}

void AND(long op1,long tipoOp1,long op2,long tipoOp2){
long valor;
    valor = getValor(op1,tipoOp1) & getValor(op2,tipoOp2);

    if (tipoOp1==1)
        Reg[op1]=valor;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor;
        }
    }

    seteaCC(valor);

}

void OR(long op1,long tipoOp1,long op2,long tipoOp2){
long valor;
    valor = getValor(op1,tipoOp1) | getValor(op2,tipoOp2);

    if (tipoOp1==1)
        Reg[op1]=valor;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor;
        }
    }

    seteaCC(valor);

}

void NOT(long op1,long tipoOp1,long op2,long tipoOp2){
long valor;
    valor = !getValor(op1,tipoOp1);

    if (tipoOp1==1)
        Reg[op1]=valor;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor;
        }
    }
    seteaCC(valor);

}

void XOR(long op1,long tipoOp1,long op2,long tipoOp2){

long valor;
    valor = getValor(op1,tipoOp1) ^ getValor(op2,tipoOp2);

    if (tipoOp1==1)
        Reg[op1]=valor;
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]=valor;
        }
        else{
            MemoriaRam[Reg[DS] + op1]=valor;
        }
    }
    seteaCC(valor);

}

void SHL(long op1,long tipoOp1,long op2,long tipoOp2){
long valor = getValor(op2,tipoOp2),resultado;

    if (tipoOp1==1){
        Reg[op1]<<=valor;
        resultado=Reg[op1];
    }
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]<<=valor;
            resultado=MemoriaRam[OperandoIndirecto(op1)];

        }
        else{
            MemoriaRam[Reg[DS] + op1]<<=valor;
            resultado=MemoriaRam[Reg[DS] + op1];
        }
    }

seteaCC(resultado);


}

void SHR(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op2,tipoOp2),resultado;

    if (tipoOp1==1){
        Reg[op1]>>=valor;
        resultado=Reg[op1];
    }
    else{
        if(tipoOp1==3){
            MemoriaRam[OperandoIndirecto(op1)]>>=valor;
            resultado=MemoriaRam[OperandoIndirecto(op1)];

        }
        else{
            MemoriaRam[Reg[DS] + op1]>>=valor;
            resultado=MemoriaRam[Reg[DS] + op1];
        }
    }

seteaCC(resultado);
}

void JMP(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op1,tipoOp1);
    Reg[IP]=(valor-1)*3;
}

void JE(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor1 = getValor(op1,tipoOp1);
    long valor2 = getValor(op2,tipoOp2);

    if (Reg[AX]==valor1)
        Reg[IP]=(valor2-1)*3;
}

void JG(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor1 = getValor(op1,tipoOp1);
    long valor2 = getValor(op2,tipoOp2);

    if (Reg[AX]<valor1)
        Reg[IP]=(valor2-1)*3;
}

void JL(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor1 = getValor(op1,tipoOp1);
    long valor2 = getValor(op2,tipoOp2);

    if (Reg[AX]>valor1)
        Reg[IP]=(valor2-1)*3;
}

void JZ(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x00000001)==0x00000001)){
        Reg[IP]=(valor-1)*3;
    }
}

void JP(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x80000000)==0x00000000) && (Reg[CC] & 0x00000001)==0x00000000){
        Reg[IP]=(valor-1)*3;
    }

}

void JN(long op1,long tipoOp1,long op2,long tipoOp2){


long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x80000000)==0x80000000) && (Reg[CC] & 0x00000001)==0x00000000){
        Reg[IP]=(valor-1)*3;
    }
}

void JNZ(long op1,long tipoOp1,long op2,long tipoOp2){


long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x00000001)==0x00000000)){
        Reg[IP]=(valor-1)*3;
    }
}

void JNP(long op1,long tipoOp1,long op2,long tipoOp2){

    long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x80000000)==0x80000000) || ((Reg[CC] & 0x00000001) == 0x00000001)){
        Reg[IP]=(valor-1)*3;
    }
}

void JNN(long op1,long tipoOp1,long op2,long tipoOp2){

long valor = getValor(op1,tipoOp1);

    if(((Reg[CC] & 0x80000000)==0x00000000) || ((Reg[CC] & 0x00000001)==0x00000001)){
        Reg[IP]=(valor-1)*3;
    }
}


void SYS(long op1,long tipoOp1,long op2,long tipoOp2){



    void read(){ // CAMBIAR
        int Celdas,largo,i;
        long Celda1,aux;
        Celdas=Reg[CX];
        Celda1=Reg[DX];
        char valor[100];

        if(Celdas!=0){
                if(Reg[AX] & 0x100 && Celdas!=-1){
                    for(i=Celda1;i<Celdas+Celda1;i++){
                        if(Reg[AX] & 0x1000){
                            printf("[%08x]:",i);
                        }
                        if(Reg[AX] & 0x0008){
                            scanf("%lx", &aux);
                        }
                        if(Reg[AX] & 0x0004){
                            scanf("%lo",&aux);
                        }
                        if(Reg[AX] & 0x0001){
                            scanf("%li",&aux);
                        }
                        MemoriaRam[Reg[DS]+i]=aux;
                    }
                }
                else{
                    i=Celda1;
                    if(Reg[AX] & 0x1000){
                        printf("[%08x]",i);
                    }
                    gets(valor);
                    largo=strlen(valor);
                    i=0;
                    if(Celdas!=-1){
                        while(i<largo && i<=Celdas-1){
                            /*if(valor[i]>=48 && valor[i]<=57){
                                valor[i]=valor[i]-48;
                            }*/
                            MemoriaRam[Reg[DS]+Celda1+i]=valor[i];
                            i++;
                        }
                    }
                    else{
                        while(i<largo){
                            MemoriaRam[Reg[DS]+Celda1+i]=valor[i];
                            i++;
                        }
                        MemoriaRam[Reg[DS]+Celda1+i]=0x0;
                    }
                }
        }
    }

    void write(int MostrarR){
        int Celdas,i,j;
        long Celda1;
        const char * const strreg[] = {"vacio","IP","CC","DS","SS","vacio","SP","BP","vacio","vacio","AX","BX","CX","DX","EX","FX"};

        Celdas=Reg[CX];
        Celda1=Reg[DX];
        i=Celda1;
        while((Celdas!=-1 && i<Celda1+Celdas) || (Celdas==-1 && MemoriaRam[Reg[DS]+i]!=0x0)){
            if(Reg[AX] & 0x1000){
                printf("[%08x]",i);
            }
            if(Reg[AX] & 0x10){
                if(MemoriaRam[Reg[DS]+i]<32 || MemoriaRam[Reg[DS]+i]==127){
                    printf(". ");
                }
                else{
                    printf("%c ",(char)(MemoriaRam[Reg[DS]+i]&0x000000FF));
                }
            }
            if(Reg[AX] & 0x8){
                printf("%c%lx ",'%',MemoriaRam[Reg[DS]+i]);
            }
            if(Reg[AX] & 0x4){
                printf("%c%lo ",'@',MemoriaRam[Reg[DS]+i]);
            }
            if(Reg[AX] & 0x2){
                fwrite(&MemoriaRam[Reg[DS]+i],sizeof(long),1,stdout);
            }
            if(Reg[AX] & 0x1){
                printf("%c%li ",'#',MemoriaRam[Reg[DS]+i]);
            }
            if(Reg[AX] & 0x100){
                printf("\n");
            }
            i++;
        }
        if(MostrarR){
            for(j=0;j<CantReg;j++){
                if(strcmp(strreg[j],"vacio")){
                    if(Reg[AX] & 0x1000){
                        printf("[%s]",strreg[j]);
                    }
                    if(Reg[AX] & 0x10){
                        if(Reg[j]<32 || Reg[j]==127){
                            printf(". ");
                        }
                        else{
                            printf("%c ",((char)Reg[j]&0xFF));
                        }
                    }
                    if(Reg[AX] & 0x8){
                        printf("%c%lx ",'%',Reg[j]);
                    }
                    if(Reg[AX] & 0x4){
                        printf("%c0%lo",'@',Reg[j]);
                    }
                    if(Reg[AX] & 0x2){
                        fwrite(&Reg[j],sizeof(long),1,stdout);
                    }
                    if(Reg[AX] & 0x1){
                        printf("%c%li",'#',Reg[j]);
                    }
                    if(Reg[AX] & 0x100){
                        printf("\n");
                    }
                }
            }
        }
    }

   void abrirArchivo(){

        vecArchivos[Reg[BX]].arch=fopen(vecArchivos[Reg[BX]].nombre,"rb+");
        if (vecArchivos[Reg[BX]].arch == NULL){
            vecArchivos[Reg[BX]].arch=fopen(vecArchivos[Reg[BX]].nombre,"wb+");
        }
        if (vecArchivos[Reg[BX]].arch == NULL)
            Reg[CC] = 0x80000000;
        else{
            Reg[CC] = 0x0;
            vecArchivos[Reg[BX]].abierto=1;
        }

    }

    void cerrarArchivo(){
        fclose(vecArchivos[Reg[BX]].arch);
        vecArchivos[Reg[BX]].abierto=0;

    }

    void readFile(){
        int i=0;
        long aux;
        FILE *arch=vecArchivos[Reg[BX]].arch;
        char Linea[Cadenamax];

        i=Reg[DS]+Reg[DX]; // NO SE SI HAY QUE SUMARLE EL DS, CREO Q NO

            if(Reg[AX]==0x0002){
                while (i<Reg[DS]+Reg[DX]+Reg[CX] && fread(&MemoriaRam[i],4,1, arch)){
                    i++;
                }
            }
            else{
                if(Reg[AX]==0x0010){
                    while (i<Reg[DS]+Reg[DX]+Reg[CX] && fread(&MemoriaRam[i],1,1, arch)){
                        i++;
                    }
                }
                else{
                    if((Reg[AX] & 0x100)==0x100){
                        if((Reg[AX] & 0xF) == 0x0){
                            while (i<Reg[DS]+Reg[DX]+Reg[CX] && fread(&aux,1,1, arch)){
                                if( (char) aux== '\n'){
                                    MemoriaRam[i]=0x0;
                                    i++;
                                }
                                else{
                                    MemoriaRam[i]=aux;
                                    i++;
                                }
                            }
                        }
                        else{
                            while (i<Reg[DS]+Reg[DX]+Reg[CX] && fgets(Linea,Cadenamax,arch)){
                                if(Reg[AX] & 0x0008){
                                    sscanf(Linea,"%lx", &aux);
                                }
                                if(Reg[AX] & 0x0004){
                                    sscanf(Linea,"%lo",&aux);
                                }
                                if(Reg[AX] & 0x0001){
                                    sscanf(Linea,"%li",&aux);
                                }
                                MemoriaRam[i]=aux;
                                i++;
                            }
                        }
                    }
                }
            }
        if (i<Reg[DS]+Reg[DX]+Reg[CX] && feof(arch)){
            Reg[CC]=0x1;
        }else
            if (i<Reg[DS]+Reg[DX]+Reg[CX] && !feof(arch))
                Reg[CC]=0x80000000;
            else
                Reg[CC]=0x0;
    }


    void writeFile(){
        int i=0;
        long aux;
        FILE *arch=vecArchivos[Reg[BX]].arch;

        i=Reg[DS]+Reg[DX]; // NO SE SI HAY QUE SUMARLE EL DS, CREO Q NO

        if(Reg[AX]==0x0002){
                while (i<Reg[DS]+Reg[DX]+Reg[CX] && fwrite(&MemoriaRam[i],4,1, arch)){
                    i++;
                }
        }
        else{
            if(Reg[AX]==0x0010){
                aux=MemoriaRam[i] & 0x000000FF;
                while (i<Reg[DS]+Reg[DX]+Reg[CX] && fwrite(&aux,1,1, arch)){
                    aux = MemoriaRam[i] & 0x000000FF;
                    i++;
                }
            }
                else{
                    if((Reg[AX] & 0x0100)==0x0100){
                        if((Reg[AX] & 0xF) == 0x0){
                            while(i<Reg[DS]+Reg[DX]+Reg[CX]){
                                while (MemoriaRam[i] != 0x0){
                                    //aux= (char) MemoriaRam[i];
                                    fprintf(vecArchivos[Reg[BX]].arch, "%c", (char)MemoriaRam[i]);
                                    i++;
                                }
                                fprintf(vecArchivos[Reg[BX]].arch, "\r\n");
                                i++;
                            }
                        }
                        else{
                            while(i<Reg[DS]+Reg[DX]+Reg[CX]){
                                if(Reg[AX] & 0x8){
                                   fprintf(arch,"%lx\r\n",MemoriaRam[i]);
                                }
                                else
                                    if(Reg[AX] & 0x4){
                                        fprintf(arch,"%lo\r\n",MemoriaRam[i]);
                                    }
                                    else
                                        if(Reg[AX] & 0x1){
                                            fprintf(arch,"%li\r\n",MemoriaRam[i]);
                                        }
                                i++;
                            }
                        }
                    }
                    if(i<Reg[DS]+Reg[DX]+Reg[CX]){ //aca no puede haber error de lectura entonces no llamo a la funcion
                        Reg[CC]=0x80000000;
                    }
                    else
                        Reg[CC] = 0x0;
                }
            }
        }



    void seekFile(){
        int pos;
        char aux[255];
        if(Reg[DX]==0)
            pos=SEEK_SET;
        else if(Reg[DX]==1)
            pos=SEEK_CUR;
        else
            pos=SEEK_END;

        if ((Reg[AX] & 0x000F) == 0x0002)
            fseek(vecArchivos[Reg[BX]].arch, (Reg[CX] * 4), pos);
        else
            if ((Reg[AX] & 0x00F0) == 0x0010)
               pos = fseek(vecArchivos[Reg[BX]].arch, Reg[CX], pos);
            else
                for (pos = 1; pos <= Reg[CX]; pos++)
                {
                    fgets(aux, 255 ,vecArchivos[Reg[BX]].arch);
                }
    }

    switch(op1){
        case 0x1:
            read();
            break;
        case 0x2:
            write(0);
            break;
        case 0x3:
            write(1);
            break;
        case 0x3D:
            abrirArchivo();
            break;
        case 0x3E:
            cerrarArchivo();
            break;
        case 0x3F:
            readFile();
            break;
        case 0x40:
            writeFile();
            break;
        case 0x42:
            seekFile();
            break;

        }
}



void STOP(long op1,long tipoOp1,long op2,long tipoOp2){
    Reg[IP]=Reg[DS];
}

void PUSH(long op1,long tipoOp1,long op2,long tipoOp2){

    if(Reg[SP]-1>=0){
        Reg[SP]--;
        MemoriaRam[Reg[SS]+Reg[SP]]=getValor(op1,tipoOp1);
    }
    else{
        printf("Stack Overflow!!\n"); //deberia terminar el programa
        STOP(0,0,0,0);
    }
}

void POP(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor;

    if(Reg[SP]<(CantMem-Reg[SS])){
        valor=MemoriaRam[Reg[SS]+Reg[SP]];
        Reg[SP]++;
        if(tipoOp1==1){
            Reg[op1]=valor;
        }
        else
            if(tipoOp1==3){
                MemoriaRam[OperandoIndirecto(op1)]=valor;
            }
            else{
                MemoriaRam[Reg[DS]+op1]=valor;
            }
    }
    else{
        printf("Pila Vacia!!\n");
        STOP(0,0,0,0);
    }

}

void PUSHALL(long op1,long tipoOp1,long op2,long tipoOp2){

    if(Reg[SP]-6>=0){
        Reg[SP]-=6;
        MemoriaRam[Reg[SS]+Reg[SP]+5]=Reg[AX];
        MemoriaRam[Reg[SS]+Reg[SP]+4]=Reg[BX];
        MemoriaRam[Reg[SS]+Reg[SP]+3]=Reg[CX];
        MemoriaRam[Reg[SS]+Reg[SP]+2]=Reg[DX];
        MemoriaRam[Reg[SS]+Reg[SP]+1]=Reg[EX];
        MemoriaRam[Reg[SS]+Reg[SP]]=Reg[FX];
    }
    else{
        printf("Stack Overflow!!\n"); //deberia terminar el programa
        STOP(0,0,0,0);
    }
}

void POPALL(long op1,long tipoOp1,long op2,long tipoOp2){
    if(Reg[SP]<=((CantMem-Reg[SS])-6)){
        Reg[SP]+=6;
        Reg[AX]=MemoriaRam[Reg[SS]+Reg[SP]-1];
       Reg[BX]=MemoriaRam[Reg[SS]+Reg[SP]-2];
        Reg[CX]=MemoriaRam[Reg[SS]+Reg[SP]-3];
        Reg[DX]=MemoriaRam[Reg[SS]+Reg[SP]-4];
        Reg[EX]=MemoriaRam[Reg[SS]+Reg[SP]-5];
        Reg[FX]=MemoriaRam[Reg[SS]+Reg[SP]-6];
    }
    else{
        printf("Pila Vacia!!\n"); //deberia terminar el programa
        STOP(0,0,0,0);
    }
}

void CALL(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor = getValor(op1,tipoOp1);
    PUSH(((Reg[IP]/3)+1),0,op2,tipoOp2);
    JMP(valor,0,op2,tipoOp2);
}

void RET(long op1,long tipoOp1,long op2,long tipoOp2){
    long valor;

    valor=MemoriaRam[Reg[SS]+Reg[SP]];
    Reg[SP]++;
    JMP(valor,0,op2,tipoOp2);
}

void SLEN(long op1,long tipoOp1,long op2,long tipoOp2){
    long contador=0,aux;

    if(tipoOp2==3){
        aux=OperandoIndirecto(op2);
    }
    else{
        aux=Reg[DS]+op2;
    }
    while(MemoriaRam[aux]!=0x0){
        contador++;
        aux++;
    }
    MOV(op1,tipoOp1,contador,0);
}

void SMOV(long op1,long tipoOp1,long op2,long tipoOp2){

    int s1, s2;

    if(tipoOp1==3){
        s1=OperandoIndirecto(op1);
    }
    else{
        s1=Reg[DS]+op1;
    }
    if(tipoOp2==3){
        s2=OperandoIndirecto(op2);
    }
    else{
        s2=Reg[DS]+op2;
    }
    while(MemoriaRam[s2]!=0x0){
        MemoriaRam[s1]=MemoriaRam[s2];
            s1++;
            s2++;
    }
    MemoriaRam[s1]=0x0;
}

void SCMP(long op1,long tipoOp1,long op2,long tipoOp2){
    int s1, s2;
    int resta = 0;

    if(tipoOp1==3){
        s1=OperandoIndirecto(op1);
    }
    else{
        s1=Reg[DS]+op1;
    }
    if(tipoOp1==3){
        s2=OperandoIndirecto(op1);
    }
    else{
        s2=Reg[DS]+op1;
    }
    while ((MemoriaRam[s2]!=0x0) && (MemoriaRam[s1]!=0x0) && (resta == 0)){
            resta = MemoriaRam[s1] - MemoriaRam[s2];
            s1++;
            s2++;
    }
    if ((MemoriaRam[s1]==0) && (MemoriaRam[+s2]!=0))
        Reg[CC] = 0x80000000;
    else
        if ((MemoriaRam[s2]==0) && (MemoriaRam[s1]!=0))
            Reg[CC] = 0x0;
        else
            if (resta==0)
                Reg[CC]=0x00000001;
            else
                if(resta>0)
                    Reg[CC]=0x0;
                else
                    Reg[CC]=0x80000000;
}

void creaNemonicos(Nemonicos registrosNemYCod[]){

strcpy(registrosNemYCod[0].etiqueta,"MOV");
registrosNemYCod[0].codigo=0x01;
registrosNemYCod[0].cantOperandos=2;
Funciones[registrosNemYCod[0].codigo]=&MOV;

strcpy(registrosNemYCod[1].etiqueta,"ADD");
registrosNemYCod[1].codigo=0x02;
registrosNemYCod[1].cantOperandos=2;
Funciones[registrosNemYCod[1].codigo]=&ADD;


strcpy(registrosNemYCod[2].etiqueta,"SUB");
registrosNemYCod[2].codigo=0x03;
registrosNemYCod[2].cantOperandos=2;
Funciones[registrosNemYCod[2].codigo]=&SUB;

strcpy(registrosNemYCod[3].etiqueta,"MUL");
registrosNemYCod[3].codigo=0x04;
registrosNemYCod[3].cantOperandos=2;
Funciones[registrosNemYCod[3].codigo]=&MUL;


strcpy(registrosNemYCod[4].etiqueta,"DIV");
registrosNemYCod[4].codigo=0x05;
registrosNemYCod[4].cantOperandos=2;
Funciones[registrosNemYCod[4].codigo]=&DIV;

strcpy(registrosNemYCod[5].etiqueta,"MOD");
registrosNemYCod[5].codigo=0x06;
registrosNemYCod[5].cantOperandos=2;
Funciones[registrosNemYCod[5].codigo]=&MOD;


strcpy(registrosNemYCod[6].etiqueta,"CMP");
registrosNemYCod[6].codigo=0x13;
registrosNemYCod[6].cantOperandos=2;
Funciones[registrosNemYCod[6].codigo]=&CMP;

strcpy(registrosNemYCod[7].etiqueta,"SWAP");
registrosNemYCod[7].codigo=0x17;
registrosNemYCod[7].cantOperandos=2;
Funciones[registrosNemYCod[7].codigo]=&SWAP;

strcpy(registrosNemYCod[8].etiqueta,"RND");
registrosNemYCod[8].codigo=0x19;
registrosNemYCod[8].cantOperandos=2;
Funciones[registrosNemYCod[8].codigo]=&RND;

strcpy(registrosNemYCod[9].etiqueta,"AND");
registrosNemYCod[9].codigo=0x31;
registrosNemYCod[9].cantOperandos=2;
Funciones[registrosNemYCod[9].codigo]=&AND;

strcpy(registrosNemYCod[10].etiqueta,"OR");
registrosNemYCod[10].codigo=0x32;
registrosNemYCod[10].cantOperandos=2;
Funciones[registrosNemYCod[10].codigo]=&OR;

strcpy(registrosNemYCod[11].etiqueta,"NOT");
registrosNemYCod[11].codigo=0x33;
registrosNemYCod[11].cantOperandos=1;
Funciones[registrosNemYCod[11].codigo]=&NOT;

strcpy(registrosNemYCod[12].etiqueta,"XOR");
registrosNemYCod[12].codigo=0x34;
registrosNemYCod[12].cantOperandos=2;
Funciones[registrosNemYCod[12].codigo]=&XOR;

strcpy(registrosNemYCod[13].etiqueta,"SHL");
registrosNemYCod[13].codigo=0x37;
registrosNemYCod[13].cantOperandos=2;
Funciones[registrosNemYCod[13].codigo]=&SHL;

strcpy(registrosNemYCod[14].etiqueta,"SHR");
registrosNemYCod[14].codigo=0x38;
registrosNemYCod[14].cantOperandos=2;
Funciones[registrosNemYCod[14].codigo]=&SHR;

strcpy(registrosNemYCod[15].etiqueta,"JMP");
registrosNemYCod[15].codigo=0x20;
registrosNemYCod[15].cantOperandos=1;
Funciones[registrosNemYCod[15].codigo]=&JMP;

strcpy(registrosNemYCod[16].etiqueta,"JE");
registrosNemYCod[16].codigo=0x21;
registrosNemYCod[16].cantOperandos=2;
Funciones[registrosNemYCod[16].codigo]=&JE;

strcpy(registrosNemYCod[17].etiqueta,"JG");
registrosNemYCod[17].codigo=0x22;
registrosNemYCod[17].cantOperandos=2;
Funciones[registrosNemYCod[17].codigo]=&JG;

strcpy(registrosNemYCod[18].etiqueta,"JL");
registrosNemYCod[18].codigo=0x23;
registrosNemYCod[18].cantOperandos=2;
Funciones[registrosNemYCod[18].codigo]=&JL;

strcpy(registrosNemYCod[19].etiqueta,"JZ");
registrosNemYCod[19].codigo=0x24;
registrosNemYCod[19].cantOperandos=1;
Funciones[registrosNemYCod[19].codigo]=&JZ;

strcpy(registrosNemYCod[20].etiqueta,"JP");
registrosNemYCod[20].codigo=0x25;
registrosNemYCod[20].cantOperandos=1;
Funciones[registrosNemYCod[20].codigo]=&JP;

strcpy(registrosNemYCod[21].etiqueta,"JN");
registrosNemYCod[21].codigo=0x26;
registrosNemYCod[21].cantOperandos=1;
Funciones[registrosNemYCod[21].codigo]=&JN;

strcpy(registrosNemYCod[22].etiqueta,"JNZ");
registrosNemYCod[22].codigo=0x27;
registrosNemYCod[22].cantOperandos=1;
Funciones[registrosNemYCod[22].codigo]=&JNZ;

strcpy(registrosNemYCod[23].etiqueta,"JNP");
registrosNemYCod[23].codigo=0x28;
registrosNemYCod[23].cantOperandos=1;
Funciones[registrosNemYCod[23].codigo]=&JNP;

strcpy(registrosNemYCod[24].etiqueta,"JNN");
registrosNemYCod[24].codigo=0x29;
registrosNemYCod[24].cantOperandos=1;
Funciones[registrosNemYCod[24].codigo]=&JNN;

strcpy(registrosNemYCod[25].etiqueta,"SYS");
registrosNemYCod[25].codigo=0x81;
registrosNemYCod[25].cantOperandos=1;
Funciones[registrosNemYCod[25].codigo]=&SYS;

strcpy(registrosNemYCod[26].etiqueta,"STOP");
registrosNemYCod[26].codigo=0x8F;
registrosNemYCod[26].cantOperandos=0;
Funciones[registrosNemYCod[26].codigo]=&STOP;

///////////////////////////// NUEVO PILA


strcpy(registrosNemYCod[27].etiqueta,"PUSH");
registrosNemYCod[27].codigo=0x44;
registrosNemYCod[27].cantOperandos=1;
Funciones[registrosNemYCod[27].codigo]=&PUSH;

strcpy(registrosNemYCod[28].etiqueta,"POP");
registrosNemYCod[28].codigo=0x45;
registrosNemYCod[28].cantOperandos=1;
Funciones[registrosNemYCod[28].codigo]=&POP;

strcpy(registrosNemYCod[29].etiqueta,"PUSHALL");
registrosNemYCod[29].codigo=0x46;
registrosNemYCod[29].cantOperandos=0;
Funciones[registrosNemYCod[29].codigo]=&PUSHALL;

strcpy(registrosNemYCod[30].etiqueta,"POPALL");
registrosNemYCod[30].codigo=0x47;
registrosNemYCod[30].cantOperandos=0;
Funciones[registrosNemYCod[30].codigo]=&POPALL;

strcpy(registrosNemYCod[31].etiqueta,"CALL");
registrosNemYCod[31].codigo=0x40;
registrosNemYCod[31].cantOperandos=1;
Funciones[registrosNemYCod[31].codigo]=&CALL;

strcpy(registrosNemYCod[32].etiqueta,"RET");
registrosNemYCod[32].codigo=0x48;
registrosNemYCod[32].cantOperandos=0;
Funciones[registrosNemYCod[32].codigo]=&RET;

///////////////////////NUEVO DE STRINGS


strcpy(registrosNemYCod[33].etiqueta,"SLEN");
registrosNemYCod[33].codigo=0x50;
registrosNemYCod[33].cantOperandos=2;
Funciones[registrosNemYCod[33].codigo]=&SLEN;

strcpy(registrosNemYCod[34].etiqueta,"SMOV");
registrosNemYCod[34].codigo=0x51;
registrosNemYCod[34].cantOperandos=2;
Funciones[registrosNemYCod[34].codigo]=&SMOV;

strcpy(registrosNemYCod[35].etiqueta,"SCMP");
registrosNemYCod[35].codigo=0x53;
registrosNemYCod[35].cantOperandos=2;
Funciones[registrosNemYCod[35].codigo]=&SCMP;



}

void OpciondeEj(int argc,const char* argv[],char rutaASM[],char rutaIMG[],Registros registros[],Nemonicos vector[],archivos vecArchivos[]){
    int HayError=1;
    int operandoResta=0;

    if(argc==1){
        printf("No se ingreso ningun archivo\n");  //CASO EN QUE SOLO SE INGRESA EL OPERADOR
    }
    else{
        iniciaRegistro(registros);
        fflush(stdin);
        creaNemonicos(vector);
        if(argc==2){
            strcpy(rutaASM,argv[1]);
            rutaIMG=NULL;
            //buscaArchs(argv,argc,vecArchivos,operandoResta);
            Lectura(rutaASM,vector,registros,&HayError);
            if(HayError)
                Ejecucion();
        }
        else{
            switch(toupper((argv[1])[1])){
            case 'T':
                strcpy(rutaASM,argv[2]);
                strcpy(rutaIMG,argv[3]);
                //buscaArchs(argv,argc,vecArchivos,operandoResta);
                Lectura(rutaASM,vector,registros,&HayError);
                if(HayError)
                    EscribeBinario(rutaIMG);
                break;
            case 'X':
                operandoResta=3;
                strcpy(rutaIMG,argv[2]);
                rutaASM=NULL;
                buscaArchs(argv,argc,vecArchivos,operandoResta);
                CargaImagen(rutaIMG);
                Ejecucion();
                break;
            case 'A':
                operandoResta=3;
                strcpy(rutaASM,argv[2]);
                rutaIMG=NULL;
                buscaArchs(argv,argc,vecArchivos,operandoResta);
                Lectura(rutaASM,vector,registros,&HayError);
                if(HayError){
                    Ejecucion();
                }
                else
                    printf("ERROR NO EJECUTA");
                break;
            }
        }
    }



}

void buscaArchs(const char* argv[],int argc,archivos vecArchivos[],int operandoResta){
    int i=0, j=operandoResta;

    while(j < argc && i < maxArchivos){
        memset(vecArchivos[i].nombre,'\0',sizeof(vecArchivos[i].nombre));
        strcpy(vecArchivos[i].nombre,argv[j]);
        //printf("ARCHIVO: %s\n",argv[j]);
        i++;
        j++;
    }
    /*memset(vecArchivos[i].nombre,'\0',sizeof(vecArchivos[i].nombre));
    strcpy(vecArchivos[i++].nombre,"des.txt");
    memset(vecArchivos[i].nombre,'\0',sizeof(vecArchivos[i].nombre));
        strcpy(vecArchivos[i++].nombre,"res.txt");*/
}


void EscribeBinario(char* RutaIMG){
    int i;
    FILE *archb=fopen(RutaIMG,"wb");

    if(archb!=NULL){
        for (i=0;i<CantReg;i++)
            fwrite(&Reg[i],sizeof(long),1,archb);
        for(i=0;i<CantMem;i++)
            fwrite(&MemoriaRam[i],sizeof(long),1,archb);
    }
    fclose(archb);
}

void CargaImagen(char* RutaIMG){
    int i;
    FILE *archb = fopen(RutaIMG,"rb");

    if(archb!=NULL){
        for(i=0;i<CantReg;i++)
            fread(&Reg[i],sizeof(long),1,archb);
        for(i=0;i<CantMem;i++){
            fread(&MemoriaRam[i],sizeof(long),1,archb);
        }
    }
    else{
        printf(" Error en la carga del archivo IMG, el archivo no existe o se ingreos mal su nombre \n");
    }
    fclose(archb);
}

void BuscaOperacion(char operacion[],Nemonicos vector[],long *cod,int *CantOperandos, int *error,FILE *arch,int *cte){
    int i=0;
    char palaux[Cadenamax];


    while(i<OperacionesMax && strcmp(operacion,vector[i].etiqueta)!=0){
        i++;
    }

    if(i!=OperacionesMax){
        *cod=vector[i].codigo;
        (*CantOperandos)=vector[i].cantOperandos;
    }
    else{
        fscanf(arch,"%s",palaux);
        strupr(palaux);
        if(!(strcmp(palaux,"EQU")==0)){
            *error=0;
        }
        else{
            printf("%s %s",operacion,palaux);
            fgets(palaux,sizeof(palaux),arch);
            printf("%s",palaux);
            *cte=0;
        }
    }

}

void Lectura(char *Archivo,Nemonicos vector[],Registros registros[],int *HayError){

    FILE *arch; //ESTOS DEBERIAN IR EN LA DECLARACION
    uint32_t posram=0;
    long cod,tipoOperador1, tipoOperador2,instruccion,operando1,operando2;
    char *Linea,VecLinea[4][40],Palabra[Cadenamax],codaux[Cadenamax],codaux2[Cadenamax],comentario[Cadenamax],*HayPtoComa,*tamaux,*auxjmp;
    int i,CantRot=0,j,CantOperandos,error=1,k,cantRotulos=0,cte=1,z;
    Labels Rot[MaxRotulos];

    memset(VecLinea[0],'\0',sizeof(VecLinea[0]));
    memset(VecLinea[1],'\0',sizeof(VecLinea[1]));
    memset(VecLinea[2],'\0',sizeof(VecLinea[2]));
    memset(VecLinea[3],'\0',sizeof(VecLinea[3]));
    memset(Palabra,'\0',sizeof(Palabra));
    memset(comentario,'\0',sizeof(comentario));
    arch=fopen(Archivo,"rt");
    if(arch==NULL){
        printf("No se pudo leer el archivo");
    }
    else{
        lecturaLabels(Archivo,Rot,&cantRotulos,HayError);
        Reg[SS]=CantMem-PilaDef;
        Reg[SP]=PilaDef;
        Reg[BP]=PilaDef;
        while(!feof(arch)){
            memset(Palabra,'\0',sizeof(Palabra));
            fscanf(arch,"%s",Palabra);
            strupr(Palabra);

            if(strcmp(Palabra,"\\\\ASM")==0){
                printf("%s",Palabra);
                fgets(Palabra,sizeof(Palabra),arch);
                printf("%s",Palabra);
                tamaux=strchr(Palabra,'=');
                tamaux++;
                Reg[SP]=strtol(tamaux,NULL,10);
                Reg[BP]=Reg[SP];
                Reg[SS]=CantMem-Reg[SP];
                memset(Palabra,'\0',sizeof(Palabra));
                fscanf(arch,"%s",Palabra);
            }
            operando1=0;
            operando2=0;
            j=0;
            Linea=Palabra;
            if(Linea[0]!='*'){ //NO ES COMENTARIO
                Linea=strchr(Palabra,':');
                if(Linea!=NULL){ //LA PRIMERA PALABRA ES ETIQUETA
                    i=0;
                    memset(codaux,'\0',sizeof(codaux));
                    while(Palabra[i]!=Linea[0]){ //MIENTRAS SEA DISTINTO DE : OBTENGO LA ETIQUETA
                        codaux[i]=Palabra[i];
                        i++;
                    }
                    CantRot++;
                    strcpy(VecLinea[j],codaux);
                    strcpy(&VecLinea[j][i],":");
                    j++;
                    if(Linea[1]!='\0'){ //SI NO HAY ESPACIO LUEGO DEL PUNTO PASO LA POSICION DE MEMORIA, EN CASO DE HABERLO LEO LA NUEVA PALABRA
                        Linea++;
                        strcpy(Palabra,Linea);
                    }
                    else{
                        fscanf(arch,"%s",Palabra);
                        strupr(Palabra);
                    }
                }
                strcpy(VecLinea[j],Palabra); // COPIO LA INSTRUCCION EN EL VECTOR DE LINEA
                BuscaOperacion(Palabra,vector,&cod,&CantOperandos,&error,arch,&cte); //OBTENGO EL CODIGO DE LA INSTRUCCION Y SU CANTIDAD DE OPERANDOS
                if(error && cte){
                    j++;
                    if(CantOperandos==1){ //1 SOLO OPERANDO
                        fgets(Palabra,sizeof(Palabra),arch);
                        strupr(Palabra);
                        Linea=strchr(Palabra,';'); //SI HAY COMENTARIO QUEDARA DELANTE DE LA SIG POSICION DE LINEA
                        if(Linea!=NULL){
                            i=1;
                            memset(codaux,'\0',sizeof(codaux));
                            while(Palabra[i]!=Linea[0] && Palabra[i]!=' ' && Palabra[i]!='\t' && Palabra[i]!='\0'){ //MIENTRAS LINEA SEA DISTINTO DE ; OBTENGO EL OPERANDO
                                codaux[i-1]=Palabra[i];
                                i++;
                            }
                            strcpy(comentario,&Palabra[i]);
                        }  // LINEA QUEDO EN ; LO SIGUIENTE SERIA COMENTARIO
                        else{
                            if(Palabra[0]=='\t' || Palabra[0]==' '){
                                z=0;
                                while(Palabra[z]=='\t' || Palabra[z]==' '){
                                    auxjmp=&Palabra[z+1];
                                    z++;
                                }
                                auxjmp[strlen(auxjmp)-1]='\0';
                                strcpy(codaux,auxjmp);
                            }
                            else{
                                Palabra[strlen(Palabra)-1]='\0';
                                strcpy(codaux,&Palabra[1]);
                            }
                        }
                        strcpy(VecLinea[j],codaux); //COPIO EL OPERANDO EN VECLINEA
                        tipoOperando(codaux,registros,&tipoOperador1);
                    }
                    else{
                        if(CantOperandos==2){ //2 OPERANDOS
                            memset(Palabra,'\0',sizeof(Palabra));
                            fscanf(arch,"%s",Palabra);
                            Linea=strchr(Palabra,',');  //busco si hay coma
                            memset(codaux,'\0',sizeof(codaux));  //reinicio el string a vacio
                            i=0;
                            if (Linea!=NULL){
                                while(Palabra[i]!=Linea[0]){   //mientra la posicion de palabra sea distinta de , sigo copiando
                                    codaux[i]=Palabra[i];
                                    i++;
                                }
                                strcpy(VecLinea[j],codaux);    //copio en veclinea el operando 1
                                strcpy(&VecLinea[j][i],",");
                                j++;
                                i++;
                            }
                            else{
                                strcpy(codaux,Palabra);
                                i=strlen(codaux);
                                strcpy(VecLinea[j],codaux);    //copio en veclinea el operando 1
                                strcpy(&VecLinea[j][i],",");
                                j++;
                            }
                            if(codaux[0]!='\''){
                                strupr(codaux);
                                strupr(VecLinea[j-1]);
                            }
                            tipoOperando(codaux,registros,&tipoOperador1);
                            if(Palabra[i]!='\0' && Palabra[i]!='\t'){ // 2 operando, HAY UN ESPACIO LUEGO DE LA COMA
                                if(Palabra[i]=='\''){       //SI VIENE UN ' VIENE UN ASCII
                                    memset(codaux2,'\0',sizeof(codaux2));
                                    k=0;
                                    codaux2[k]=Palabra[i];i++;k++;    //COPIO SI O SI 2 CARACTERES QUE SERAN ' SEGUIDO DE UN CARACTER ASCII
                                    codaux2[k]=Palabra[i];i++;k++;
                                    while(Palabra[i]!='\0' && Palabra[i]!=';' && Palabra[i]!='\''){    //COPIO HASTA QUE SE DETENGA
                                        codaux2[k]=Palabra[i];i++;k++;
                                    }
                                    if(Palabra[i]=='\''){
                                        strcpy(&codaux2[k],"'");
                                    }
                                    if(Palabra[i]=='\0'){
                                        fgets(Palabra,sizeof(Palabra),arch);
                                        if(Palabra[1]=='\''){
                                            strcpy(codaux2,"\' \'");
                                        }
                                        strcpy(&codaux2[k],&Palabra[0]);
                                    }
                                    HayPtoComa=strchr(&Palabra[i],';');      //SI SE DETIENE POR UN ESPACIO, VERIFICO QUE LUEGO HAYA O NO UN ;
                                    if(HayPtoComa!=NULL){ // COMENTARIO
                                        strcpy(comentario,&Palabra[i]);
                                    }
                                }
                                else{
                                    k=0;
                                    strupr(Palabra);
                                    memset(codaux2,'\0',sizeof(codaux2));    // SIMILAR AL ANTERIOR
                                    while(Palabra[i]!='\0' && Palabra[i]!=';'){
                                        codaux2[k]=Palabra[i];i++;k++;
                                    }
                                    if(Palabra[i]==';'){
                                        strcpy(comentario,&Palabra[i]);
                                    }
                                    else{
                                        fgets(Palabra,sizeof(Palabra),arch);
                                        HayPtoComa=strchr(Palabra,';');
                                        if(HayPtoComa!=NULL){        // COMENTARIO
                                            strcpy(comentario,Palabra);
                                        }
                                    }
                                }
                                strcpy(VecLinea[j],codaux2);
                            }
                            else{
                                fscanf(arch,"%s",Palabra);    //EN ESTE CASO LEE LA PALABRA
                                i=0;
                                if(Palabra[0]==',')
                                    strcpy(Palabra,&Palabra[1]);
                                if(Palabra[0]=='\0')
                                    fscanf(arch,"%s",Palabra);
                                if(Palabra[i]=='\''){      //SI HAY UN ' ES UN ASCII  Y ACTUA SIMILAR A LO ANTERIOR
                                    memset(codaux2,'\0',sizeof(codaux2));
                                    codaux2[i]=Palabra[i];i++;
                                    codaux2[i]=Palabra[i];i++;
                                    while(Palabra[i]!='\0' && Palabra[i]!=';' && Palabra[i]!='\''){
                                        codaux2[k]=Palabra[i];i++;
                                    }
                                    if(Palabra[i]=='\''){
                                        strcpy(&codaux2[i],"\'");
                                        i++;
                                    }
                                    HayPtoComa=strchr(&Palabra[i],';');
                                    if(HayPtoComa!=NULL){   //COMENTARIO
                                        strcpy(comentario,&Palabra[i]);
                                    }
                                }
                                else{
                                    strupr(Palabra);
                                    memset(codaux2,'\0',sizeof(codaux2));
                                    while(Palabra[i]!='\0' && Palabra[i]!=';'){
                                        codaux2[i]=Palabra[i];i++;
                                    }
                                    fgets(Palabra,sizeof(Palabra),arch);
                                    Linea=strchr(Palabra,';');
                                    if(Linea!=NULL){  //COMENTARIO
                                        strcpy(comentario,&Palabra[i]);
                                    }
                                }
                                strcpy(VecLinea[j],codaux2);
                            }
                                tipoOperando(codaux2,registros,&tipoOperador2);   //INTERPRETA EL TIPO DEL 2 OPERANDO
                        }
                    }
                    instruccion=DevuelveInstruccion(cod,tipoOperador1,tipoOperador2);
                    MemoriaRam[posram]=instruccion;
                    posram++;
                   // printf(" %08x \n",instruccion);
                    if (CantOperandos!=0){
                        operando1=DevuelveOperando(cod,tipoOperador1,codaux,Rot,cantRotulos,HayError); // transforma el char y devuelve el operando como int
                      //  printf("%x \n",operando1);
                        if(CantOperandos==2){
                            operando2=DevuelveOperando(cod,tipoOperador2,codaux2,Rot,cantRotulos,HayError); // igual al otro
                           // printf("%x \n",operando2);
                        }
                    }
                    MemoriaRam[posram]=operando1;
                    posram++;
                    MemoriaRam[posram]=operando2;
                    posram++;
                    comentario[strlen(comentario)-1]='\0';
                    printf("[%08x]: %08lx %08lx %08lx ",posram-3,instruccion,operando1,operando2);
                    printf("%d:  %s %s %s %s %s \n",(posram/3),VecLinea[0],VecLinea[1],VecLinea[2],VecLinea[3],comentario);
                }
                else{
                    if(error==0){
                        printf("Error, la instruccion %s no es valida \n",Palabra);
                        fgets(Palabra,sizeof(Palabra),arch);
                        MemoriaRam[posram]=0xFFFFFFFF;
                        posram++;
                        MemoriaRam[posram]=0xFFFFFFFF;
                        posram++;
                        MemoriaRam[posram]=0xFFFFFFFF;
                        posram++;
                        error=1;
                        *HayError=0;
                        printf("[%08x]: %08lx %08lx %08lx \t",posram-3,MemoriaRam[posram-3],MemoriaRam[posram-2],MemoriaRam[posram-1]);
                        printf("%d:  %s %s  \n",(posram/3),VecLinea[0],Palabra);
                    }
                    else{
                        cte=1;
                    }
                }
            }
            else{
                strcpy(comentario,Palabra);
                fgets(Palabra,sizeof(Palabra),arch);
                if(Palabra[0]!='\n'){
                    strcat(comentario,Palabra);
                }
                printf("%s",comentario);
            }
            memset(VecLinea[0],'\0',sizeof(VecLinea[0]));
            memset(VecLinea[1],'\0',sizeof(VecLinea[1]));
            memset(VecLinea[2],'\0',sizeof(VecLinea[2]));
            memset(VecLinea[3],'\0',sizeof(VecLinea[3]));
            memset(comentario,'\0',sizeof(comentario));
            tipoOperador1=0;
            tipoOperador2=0;
        }
        Reg[DS]=posram;
    }
    fclose(arch);
}


long DevuelveInstruccion(long cod,long tipoOperando1,long tipoOperando2){

    cod=cod << 16;
    tipoOperando1 = tipoOperando1 << 4;

    return cod | tipoOperando1 | tipoOperando2;
}

long DevuelveOperando(long cod,long tipoOperando,char codaux[],Labels Rot[],long cantRotulos,int *HayError){

    char *aux,*mas,*menos,auxiliar[20];
    long retorno,x;
    int i;


    if(tipoOperando==3){
        mas=NULL;
        menos=NULL;
        mas=strchr(codaux,'+');
        menos=strchr(codaux,'-');
        memset(auxiliar,'\0',sizeof(auxiliar));
        if(mas!=NULL || menos!=NULL){
            i=1;
            while(codaux[i]!='+' && codaux[i]!='-'){
                auxiliar[i-1]=codaux[i];
                i++;
            }
            if (strcmp(auxiliar,"AX")==0)
                retorno=AX;
            else
                if (strcmp(auxiliar,"BX")==0)
                    retorno=BX;
                else
                    if (strcmp(auxiliar,"CX")==0)
                        retorno=CX;
                    else
                        if (strcmp(auxiliar,"DX")==0)
                            retorno=DX;
                        else
                            if (strcmp(auxiliar,"EX")==0)
                                retorno=EX;
                            else
                                if (strcmp(auxiliar,"FX")==0)
                                    retorno=FX;
                                else
                                    if (strcmp(auxiliar,"SP")==0)
                                        retorno=SP;
                                    else
                                        if (strcmp(auxiliar,"BP")==0)
                                            retorno=BP;

            if(mas!=NULL){
                aux=mas+1;
                if(aux[0]>='0' && aux[0]<='9')
                    x=strtol(aux,NULL,10);
                else{
                    i=0;
                    while(aux[i]!=']'){
                        auxiliar[i]=aux[i];
                        i++;
                    }
                    //strupr(auxiliar);
                    x=busquedaLabel(Rot,auxiliar,cantRotulos,HayError);
                }
                x <<= 8;
                retorno=(retorno | x);
            }
            else{
                aux=menos+1;
                if(aux[0]>='0' && aux[0]<='9')
                    x=strtol(aux,NULL,10);
                else{
                    i=0;
                    while(aux[i]!=']'){
                        auxiliar[i]=aux[i];
                        i++;
                    }
                    //strupr(auxiliar);
                    x=busquedaLabel(Rot,auxiliar,cantRotulos,HayError);
                }
                x <<= 8;
                x=x*-1;
                retorno=(retorno | x);
                //retorno=(retorno | 80000000);
            }

        }
        else{
            i=1;
            while(codaux[i]!=']'){
                auxiliar[i-1]=codaux[i];
                i++;
            }


           if (strcmp(auxiliar,"AX")==0)
                retorno=AX;
            else
                if (strcmp(auxiliar,"BX")==0)
                    retorno=BX;
                else
                    if (strcmp(auxiliar,"CX")==0)
                        retorno=CX;
                    else
                        if (strcmp(auxiliar,"DX")==0)
                            retorno=DX;
                        else
                            if (strcmp(auxiliar,"EX")==0)
                                retorno=EX;
                            else
                                if (strcmp(auxiliar,"FX")==0)
                                    retorno=FX;
                                else
                                    if (strcmp(auxiliar,"SP")==0)
                                        retorno=SP;
                                    else
                                        if (strcmp(auxiliar,"BP")==0)
                                            retorno=BP;

        }
    }
    else{
        if(tipoOperando==2){ //directo  DIRECCION DE MEMORIA
            aux=strtok(codaux+1,"]");
            retorno=strtol(aux,NULL,10);
        }
        else{
            if(tipoOperando==1){ //de registro  DIRECCION DE REGISTRO
                if(!strcmp(codaux,"IP")){
                    retorno=IP;
                }
                else{
                    if(!strcmp(codaux,"CC")){
                        retorno=CC;
                    }
                    else{
                        if(!strcmp(codaux,"DS")){
                            retorno=DS;
                        }
                        else{
                            if(!strcmp(codaux,"SS")){
                                retorno=SS;
                            }
                            else{
                                if(!strcmp(codaux,"SP")){
                                    retorno=SP;
                                }
                                else{
                                    if(!strcmp(codaux,"BP")){
                                        retorno=BP;
                                    }
                                    else
                                        retorno=toupper(codaux[0])-55;
                                }
                            }
                        }
                    }
                }
            }
            else{ //inmediato
                switch(codaux[0]){  //NUMERO O CARACTER
                case '#':
                    sscanf(codaux+1,"%ld",&retorno);
                    break;
                case '\'':
                    retorno = (long) codaux[1];
                    break;
                case '@':
                    sscanf(codaux+1,"%lo",&retorno);
                    break;
                case '%':
                    sscanf(codaux+1,"%lx",&retorno);
                    break;
                case '0' ... '9':
                    retorno=atoi(codaux);
                    break;
                case'-':
                    retorno=atoi(codaux);
                    break;
                default:
                    retorno=busquedaLabel(Rot,codaux,cantRotulos,HayError);
                }
            }
        }
    }
    return retorno;
}


void lecturaLabels (char* Archivo,Labels Rotulos[],int *cantRotulos,int *HayError){
FILE *arch;
int i,k,NroLinea=1;
char Linea[Cadenamax],Linea2[Cadenamax],codaux[Cadenamax],*HayDosPuntos;
char DosPuntos= ':',*HayPuntoyComa;

arch=fopen(Archivo,"rt");

if(arch==NULL){
        printf("No se pudo leer el archivo");
    }
    else{
        memset(Linea,'\0',sizeof(Linea));
        fscanf(arch,"%s",Linea);
        strupr(Linea);
        while(!feof(arch)){
            if (Linea[0]!='*' && strcmp(Linea,"\\\\ASM")!=0){ //NO ES UN COMENTARIO
                HayPuntoyComa=strchr(Linea,';');
                if(HayPuntoyComa!=NULL){
                    fgets(Linea,sizeof(Linea),arch);
                    fscanf(arch,"%s",Linea);
                }
                else{
                i=strlen(Linea);
                if (Linea[i-1]==':'){
                    memset(Rotulos[*cantRotulos].Etiqueta,'\0',sizeof(Rotulos[*cantRotulos].Etiqueta));
                    strupr(Linea);
                    for (k=0;k<i-1;k++)
                       Rotulos[*cantRotulos].Etiqueta[k]= Linea[k];
                    Rotulos[*cantRotulos].codigo=NroLinea;
                    (*cantRotulos)++;
                }
                else{
                    i=0;
                    HayDosPuntos = strchr(Linea, DosPuntos);
                    while (HayDosPuntos!= NULL && i<strlen(Linea) && Linea[i]!=*HayDosPuntos) //BUSCO POSICION DEL :
                        i++;
                    if (HayDosPuntos!=NULL){ //ES UNA ETIQUETA DE SALTO, COPIO EL NOMBRE DEL ROTULO
                        memset((Rotulos)[*cantRotulos].Etiqueta,'\0',sizeof((Rotulos)[*cantRotulos].Etiqueta));
                        strupr(Linea);
                        for (k=0;k<i;k++){
                            (Rotulos)[*cantRotulos].Etiqueta[k]= Linea[k];
                        }

                        (Rotulos)[*cantRotulos].codigo=NroLinea; //AL ROTULO LE ASIGNO LA DIRECCION A LA CUAL TIENE QUE SALTAR
                        (*cantRotulos)++;
                    }
                    else{
                        if(strcmp("STOP",Linea)!=0 && strcmp("PUSHALL",Linea)!=0 && strcmp("POPALL",Linea)!=0){
                            memset(Linea2,'\0',sizeof(Linea2));
                            fscanf(arch,"%s",Linea2);
                            strupr(Linea2);
                            //ANALIZO QUE ES UNA CONSTANTE
                            if (strcmp(Linea2,"EQU")==0){
                                for (i=0;i<*cantRotulos;i++){
                                    if (strcmp(Rotulos[*cantRotulos].Etiqueta,Linea)==0){
                                        *HayError=0;
                                        printf(" \n Hay error, %s ya existe.",Linea);
                                    }
                                }
                                strcpy(Rotulos[*cantRotulos].Etiqueta,Linea);
                                memset(codaux,'\0',sizeof(codaux));
                                fscanf(arch, "%s",codaux);
                                switch(codaux[0]){  //NUMERO O CARACTER
                                    case '#':
                                        sscanf(codaux+1,"%d",&(Rotulos[*cantRotulos].codigo));
                                        break;
                                    case '@':
                                        sscanf(codaux+1,"%o",&(Rotulos[*cantRotulos].codigo));
                                        break;
                                    case '%':
                                        sscanf(codaux+1,"%x",&(Rotulos[*cantRotulos].codigo));
                                        break;
                                    case '0' ... '9':
                                        Rotulos[*cantRotulos].codigo=atoi(codaux);
                                        break;
                                    case'-':
                                        Rotulos[*cantRotulos].codigo=atoi(codaux);
                                        break;
                                    default:;
                                }
                                (*cantRotulos)++;
                                NroLinea--;
                            }
                        }
                    }

                }
                NroLinea++;
            }
            }
            if ((strcmp(Linea2,"EQU")!=0 && strcmp(Linea,"STOP")!=0 && strcmp(Linea,"POPALL")!=0 && strcmp(Linea,"PUSHALL")!=0) || (Linea[0]=='*') || strcmp(Linea,"\\\\ASM")==0)
                fgets(Linea,sizeof(Linea),arch);
            memset(Linea,'\0',sizeof(Linea));
            fscanf(arch," %s ",Linea);
            strupr(Linea);
        }
        /*for(i=0;i<*cantRotulos;i++){
            printf("%s %d \n",Rotulos[i].Etiqueta,Rotulos[i].codigo);
        }*/
        fclose(arch);
    }

}

int busquedaLabel (Labels Rotulos[],char etiqueta[],int cantRotulos,int * HayError){
    int i=0;
    while (i<cantRotulos && strcmp(etiqueta,Rotulos[i].Etiqueta))
        i++;

    if (i<cantRotulos)
        return Rotulos[i].codigo;
    else{
        *HayError=0;
        return -1;
    }
}



void Ejecucion(){
    Reg[IP]=0;
    long aux,aux1,aux2;

    while(0<=Reg[IP] && Reg[IP]<Reg[DS]){
        aux=MemoriaRam[Reg[IP]];
        aux=aux>>16;
        aux1=MemoriaRam[Reg[IP]] & 0xF0;
        aux1= aux1 >> 4;
        aux2=MemoriaRam[Reg[IP]] & 0xF;
        Reg[IP]+=3;
        if(aux!=-1){
            Funciones[aux](MemoriaRam[Reg[IP]-2],aux1,MemoriaRam[Reg[IP]-1],aux2);

        }
    }
}

int main(int argc,const char *argv[])
{


    char rutaASM[Nommax],rutaIMG[Nommax];
    Nemonicos registrosNemYCod[256];
    Registros registros[CantReg];


    for(int i=0;i<maxArchivos;i++){
        vecArchivos[i].abierto=0;
    }
    printf(" ARCHIVO: %s",argv[4]);
    OpciondeEj(argc,argv,rutaASM,rutaIMG,registros,registrosNemYCod,vecArchivos);
    return 0;

}






