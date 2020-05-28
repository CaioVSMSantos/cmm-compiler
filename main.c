/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
//#include <TygAnalex.h>
#include <TygSyntactic.h>

int main(){
    printf("\n   -= TYG Cmm COMPILER =-\n\n");
    openFile();
    initializeAnalexVariables();
    initializeSemanticVariables();
    initializeCodeGenVariables();

    activateTygCompiler();

    closeFile();
    system("PAUSE");
    return 0;
}
