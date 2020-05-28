/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <TygSemantic.h>

void initializeSemanticVariables(){
    printf("\n  Loading Semantic Variables...\n");
    tblSymbolsCurrentIndex = 0;
    currentType = semretorno;
    currentScope = GLOBAL;
    currentGroup = VARIABLE;
    funcTempType = 0;
    funcPointer = 0;
    paramPointer = 0;
    returnTrigger = 0;
    returnType = 0;
    funcReturnType = 0;
    mainFunction = 0;
    printf("  Finished\n\n");
}

void semanticError(int errorCod){
    printf("\n - - - - - - - \n\n");
    printf("Semantic Error!\n\n");
    switch (errorCod) {
        case CONFLICTING_PARAMETER_NAME:
            printf("Conflicting Parameter name found at line %d!\n",programLines);
            break;
        case CONFLICTING_FUNCTION_NAME:
            printf("Conflicting Function name found at line %d!\n",programLines);
            break;
        case CONFLICTING_VARIABLE_NAME:
            printf("Conflicting Variable name found at line %d!\n",programLines);
            break;
        case FUNCTION_BEFORE_PROTOTYPE:
            printf("Function declaration before Prototype at line %d!\n",programLines);
            break;
        case PROT_FUNC_DIFFERENT_RETURN_TYPES:
            printf("Function and Prototype have different return types at line %d!\n",programLines);
            break;
        case PROT_FUNC_DIFFERENT_PARAMETERS:
            printf("Function and Prototype have different Parameters at line %d!\n",programLines);
            break;
        case PROT_FUNC_PARAM_DIFFERENT_LEXEME:
            printf("Function and Prototype Parameters have different lexeme at line %d!\n",programLines);
            break;
        case PROT_FUNC_PARAM_DIFFERENT_TYPES:
            printf("Function and Prototype Parameters have different Types at line %d!\n",programLines);
            break;
        case VARIABLE_NOT_DECLARED:
            printf("Variable without declaration found at line %d!\n",programLines);
            break;
        case FUNCTION_NOT_DECLARED:
            printf("Function without declaration found at line %d!\n",programLines);
            break;
        case FUNCTION_CALL_WRONG_PARAMETERS:
            printf("Function call with wrong Parameters at line %d!\n",programLines);
            break;
        case INCOMPATIBLE_EXPRESSION_TYPES:
            printf("Expression have incompatible types and/or operands at line %d!\n",programLines);
            break;
        case MISSING_RETURN:
            printf("Missing a 'Retorne' command in function at line %d!\n",programLines);
            break;
        case RETURN_IN_VOID_FUNCTION:
            printf("'Retorne' command in 'Semretorno' function at line %d!\n",programLines);
            break;
        case WRONG_RETURN_TYPE:
            printf("Wrong type of 'retorne' expression in function at line %d!\n",programLines);
            break;
        case VOID_FUNCTION_EXPECTED:
            printf("Expected 'semretorno' function at line %d!\n",programLines);
            break;
        case NO_VOID_FUNCTION_EXPECTED:
            printf("Expected a function with return type other than 'senretorno' at line %d!\n",programLines);
            break;
        case MAIN_FUNCTION_NOT_FOUND:
            printf("Main Function 'principal' not found at line %d!\n",programLines);
            break;
            /*
        case 1:
            printf("  %d!\n",programLines);
            break;
            */
    }
    printf("\n - - - - - - - \n\n");
    printf("--   Warning! Machine Code Interrupted!   --\n\n");
    codeGen("<CODE INTERRUPTED>");
    closeCodeFile();
    closeFile();
    system("PAUSE");
    exit(1);
}

int checkSymbolConflict(char lexema[50], scope scp, group grp){
    int idx = tblSymbolsCurrentIndex-1;
    while(idx >= 0){
        if((tblSymbols[idx].scp == scp) &&
           (strcmp(lexema,tblSymbols[idx].lexema) == 0) &&
           (strcmp(lexema,"_NULL") != 0)&&
           (tblSymbols[idx].zmb == FALSE) &&
           (grp != FUNCTION || tblSymbols[idx].grp != PROTOTYPE)) return 1;
        idx--;
    }
    return 0;
}

void storeSymbol(int idx, char lexema[50], int type, scope scp, group grp, zombie zmb){
    if(strcmp(lexema,"_NULL") != 0){
        strcpy(tblSymbols[idx].lexema, lexema);
    }
    if(type != -1){
        tblSymbols[idx].type = type;
    }
    if(scp != -1){
        tblSymbols[idx].scp = scp;
    }
    if(grp != -1){
        tblSymbols[idx].grp = grp;
    }
    if(zmb != -1){
        tblSymbols[idx].zmb = zmb;
    }
 //   printf("   %d - %s - %d - %d - %d \n",tblSymbolsCurrentIndex,tblSymbols[tblSymbolsCurrentIndex].lexema,tblSymbols[tblSymbolsCurrentIndex].type,tblSymbols[tblSymbolsCurrentIndex].scp,
 //          tblSymbols[tblSymbolsCurrentIndex].grp);
}

void zombifyLocalParameters(){
    int idx = tblSymbolsCurrentIndex-1;
    while(tblSymbols[idx].scp == LOCAL){
        tblSymbols[idx].zmb = TRUE;
        idx--;
    }
}

void destroyLocalVariables(){
    int idx = tblSymbolsCurrentIndex-1;
    while(tblSymbols[idx].scp == LOCAL && tblSymbols[idx].grp == VARIABLE){
        strcpy(tblSymbols[idx].lexema, "_NULL");
        tblSymbols[idx].grp = -1;
        tblSymbols[idx].scp = -1;
        tblSymbols[idx].type = -1;
        tblSymbols[idx].zmb = -1;

        tblSymbolsCurrentIndex--;
    }
}


//Call this right after finding a function and storing its parameters
void comparePrototypeFunction(char lexema[50], int type){
    int protInd = -1;
    int funcInd = -1;
    int idx = 0;

    while(idx <= tblSymbolsCurrentIndex){
        if((strcmp(lexema,tblSymbols[idx].lexema) == 0) && (tblSymbols[idx].grp == PROTOTYPE)){
            protInd = idx;
        }else if((strcmp(lexema,tblSymbols[idx].lexema) == 0) && (tblSymbols[idx].grp == FUNCTION)){
            funcInd = idx;
        }
        idx++;
    }
    if(protInd != -1){
        if(funcInd < protInd){
            semanticError(FUNCTION_BEFORE_PROTOTYPE);
        }
        if(tblSymbols[protInd].type != tblSymbols[funcInd].type){
            semanticError(PROT_FUNC_DIFFERENT_RETURN_TYPES);
        }
        protInd++;
        funcInd++;
        while(tblSymbols[protInd].grp == PARAMETER){
            if(tblSymbols[funcInd].grp != PARAMETER){
                semanticError(PROT_FUNC_DIFFERENT_PARAMETERS);
            }
            if(tblSymbols[protInd].type != tblSymbols[funcInd].type){
               semanticError(PROT_FUNC_PARAM_DIFFERENT_TYPES);
            }
            if(strcmp("_NULL",tblSymbols[protInd].lexema) != 0){
                if(strcmp(tblSymbols[funcInd].lexema,tblSymbols[protInd].lexema) != 0){
                    semanticError(PROT_FUNC_PARAM_DIFFERENT_LEXEME);
                }
            }
            protInd++;
            funcInd++;
        }
    }
}

//Also returns the variable Type if it exists
int checkVariablePreviousDeclaration(char lexema[50]){
    //Searching for a LOCAL Declaration First
    int idx = tblSymbolsCurrentIndex-1;
    while(tblSymbols[idx].grp == VARIABLE || idx >= 0){
        if((strcmp(tblSymbols[idx].lexema,lexema) == 0) && tblSymbols[idx].zmb == FALSE){
           return tblSymbols[idx].type;
        }
        idx--;
    }
    //Searching for a GLOBAL Declaration
    idx = 0;
    while(idx <= tblSymbolsCurrentIndex){
        if((strcmp(tblSymbols[idx].lexema,lexema) == 0) &&
           tblSymbols[idx].scp == GLOBAL &&
           tblSymbols[idx].grp == VARIABLE){
                return tblSymbols[idx].type;
           }
           idx++;
    }
    return -1;
}

//Also returns the function Type if it exists
int checkFunctionPreviousDeclaration(char lexema[50]){
    //Searching for a PROTOTYPE of FUNCTION
    int idx = 0;
    while(idx <= tblSymbolsCurrentIndex){
        if((strcmp(tblSymbols[idx].lexema,lexema) == 0) &&
           (tblSymbols[idx].grp == PROTOTYPE || tblSymbols[idx].grp == FUNCTION)){
                return tblSymbols[idx].type;
           }
            idx++;
    }
    return -1;
}

int setFunctionParametersCheckPointers(char lexema[50]){
    funcPointer = 0;
    while(funcPointer <= tblSymbolsCurrentIndex){
        if((strcmp(tblSymbols[funcPointer].lexema,lexema) == 0) &&
           (tblSymbols[funcPointer].grp == FUNCTION || tblSymbols[funcPointer].grp == PROTOTYPE)){
                paramPointer = funcPointer+1;
                return 1;
           }
           funcPointer++;
    }
    return 1;
}

void compareParameters(int type){
    if(tblSymbols[paramPointer].grp == PARAMETER){
        switch(tblSymbols[paramPointer].type){
            case caracter:
                if(type != caracter && type != inteiro){
                    semanticError(FUNCTION_CALL_WRONG_PARAMETERS);
                }
                break;
            case inteiro:
                if(type != inteiro && type != caracter && type != booleano){
                    semanticError(FUNCTION_CALL_WRONG_PARAMETERS);
                }
                break;
            case real:
                if(type != real){
                    semanticError(FUNCTION_CALL_WRONG_PARAMETERS);
                }
                break;
            case booleano:
                if(type != inteiro && type != booleano){
                    semanticError(FUNCTION_CALL_WRONG_PARAMETERS);
                }
                break;
        }
    }else semanticError(FUNCTION_CALL_WRONG_PARAMETERS);
    paramPointer++;
}
