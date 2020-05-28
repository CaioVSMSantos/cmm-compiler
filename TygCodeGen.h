/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *fc;
int multVarCont;
int currentFuncVarCont;
char tmpStr[20];
char tmpLex[50];
char FL[10];
char lastLabel[10];
int currentRegister;
int currentRelativeAddress;
int labelIndex;
typedef struct {
    char func[50];
    char lbl[5];
} funcLabel;

funcLabel tblFuncLabel[1000];
int tblFuncLabelCurrentIndex;

void initializeCodeFile();
void closeCodeFile();
void codeGen(char cd[50]);
void initializeCodeGenVariables();
void generateLabel();
int getVarRegMem(char lexema[50]);
void alignFuncLabel(char lexema[50],char label[10]);
