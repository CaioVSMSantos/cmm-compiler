/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/

#include <TygCodeGen.h>
#include <TygSemantic.h>

void initializeCodeFile(){
    if((fc = fopen("CmmGeneratedCode.txt","w")) == NULL) {
        printf("\n   Unexpected error when creating Code file. Exiting...");
        system("PAUSE");
        exit(1);
    }
}

void closeCodeFile(){
    fclose(fc);
}

void codeGen(char cd[50]){
    fprintf(fc,cd);
    fprintf(fc,"\n");
}

void initializeCodeGenVariables(){
    printf("\n  Loading Code Generator Variables...\n");
    multVarCont = 0;
    currentFuncVarCont = 0;
    labelIndex = 1;
    currentRegister = 0;
    currentRelativeAddress = 0;
    tblFuncLabelCurrentIndex = 0;
    printf("  Finished\n\n");
}

void generateLabel(){
    char tmp[5];
    sprintf(tmp,"L%d",labelIndex);
    labelIndex++;
    strcpy(lastLabel,tmp);
}

int getVarRegMem(char lexema[50]){
    int idx = tblSymbolsCurrentIndex-1;
    int tidx, pcount, ppos;
    while(tblSymbols[idx].grp != FUNCTION && idx >= 0){
        idx--;
    }
    idx++;
    //Searching for a Parameter
    currentRegister = 1;
    currentRelativeAddress = 0;
    tidx = idx;
    pcount = 1;
    ppos = 0;
    while(tblSymbols[tidx].grp == PARAMETER){
        if(strcmp(tblSymbols[tidx].lexema,lexema) == 0){
            ppos = pcount;
        }
        tidx++;
        pcount++;
    }
    if(ppos > 0){
        currentRelativeAddress = -2-pcount+ppos;
        return 1;
    }
    //Searching for a LOCAL Declaration
    currentRelativeAddress = 0;
    while(tblSymbols[idx].grp == VARIABLE && tblSymbols[idx].scp == LOCAL){
        if(strcmp(tblSymbols[idx].lexema,lexema) == 0){
            currentRegister = 1;
            return 1;
        }
        idx++;
        currentRelativeAddress++;
    }
    //Searching for a GLOBAL Declaration
    idx = 0;
    currentRegister = 0;
    currentRelativeAddress = 0;
    while(idx <= tblSymbolsCurrentIndex){
        if((strcmp(tblSymbols[idx].lexema,lexema) == 0) &&
           tblSymbols[idx].scp == GLOBAL &&
           tblSymbols[idx].grp == VARIABLE){
                currentRegister = 0;
                return 1;
           }
           currentRelativeAddress++;
           idx++;
    }
    return 0;
}

void alignFuncLabel(char lexema[50],char label[10]){
    strcpy(tblFuncLabel[tblFuncLabelCurrentIndex].func,lexema);
    strcpy(tblFuncLabel[tblFuncLabelCurrentIndex].lbl,label);
    tblFuncLabelCurrentIndex++;
}

void getFuncLabel(char lexema[50]){
    int idx = 0;
    while(idx <= tblFuncLabelCurrentIndex){
        if(strcmp(tblFuncLabel[idx].func,lexema) == 0){
            strcpy(FL,tblFuncLabel[idx].lbl);
        }
        idx++;
    }
}
