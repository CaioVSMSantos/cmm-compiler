/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <TygSyntactic.h>


void activateTygCompiler(){
    parseTokens();  //Initial parse to fill nextTkn.
    printf("\nPerforming Code Analysis...\n");

    initializeCodeFile();
    codeGen("INIP");
    prog();
    codeGen("HALT");
    closeCodeFile();

    printf("Analysis Complete. Code is correct!!\n");
    printf("\n   Complete Machine Code Generated!\n\n");
}
/*
void initializeSyntacticVariables(){
    printf("\n  Loading Syntactic Variables...\n");
    printf("  Finished\n\n");
}*/

void syntacticError(int errorCod){
    printf("\n - - - - - - - \n\n");
    printf("Syntactic Error!\n\n");
    switch (errorCod) {
        case WRONG_START_STATEMENT: //CHECKED
            printf("Wrong start of statement at line %d!\n",programLines);
            printf("Expected Type, 'semretorno' or 'prototipo'.\n");
            break;
        case ID_AFTER_TYPE_EXPECTED: //CHECKED
            printf("Identifier expected after Type at line %d!\n",programLines);
            break;
        case MISSING_DECL_PONCTUATION: //CHECKED
            printf("Missing '(', ',' or ';' at line %d!\n",programLines);
            break;
        case ID_AFTER_COMMA_EXPECTED: //CHECKED
            printf("Identifier expected after ',' at line %d!\n",programLines);
            break;
        case PROTOTYPE_TYPE_EXPECTED: //CHECKED
            printf("Type or 'semretorno' expected at line %d!\n",programLines);
            break;
        case OPEN_PARENTHESIS_EXPECTED: //CHECKED
            printf("'(' expected at line %d!\n",programLines);
            break;
        case CLOSE_PARENTHESIS_EXPECTED: //CHECKED
            printf("')' expected at line %d!\n",programLines);
            break;
        case OPEN_CURLY_BRACKETS_EXPECTED: //CHECKED
            printf("'{' expected at line %d!\n",programLines);
            break;
        case CLOSE_CURLY_BRACKETS_EXPECTED: //CHECKED
            printf("'}' expected at line %d!\n",programLines);
            break;
        case SEMICOLON_MISSING: //CHECKED
            printf("';' missing at line %d!\n",programLines);
            break;
        case PARAM_TYPE_EXPECTED: //CHECKED
            printf("Parameter Type expected at line %d!\n",programLines);
            break;
        case TYPE_EXPECTED: //CHECKED
            printf("Type expected at line %d!\n",programLines);
            break;
        case UNEXPECTED_SYMBOL: //CHECKED
            printf("Unexpected Symbol found at line %d!\n",programLines);
            break;
        case FACTOR_EXPECTED: //CHECKED
            printf("Wrong or Missing Factor at line %d!\n",programLines);
            break;
        case ATRIB_EXPECTED: //CHECKED
            printf("'=' expected at line %d!\n",programLines);
            break;
    }
    printf("\n - - - - - - - \n\n");
    printf("--   Warning! Machine Code Interrupted!   --\n\n");
    codeGen("<CODE INTERRUPTED>");
    closeCodeFile();
    closeFile();
    system("PAUSE");
    exit(1);
}

int isTipo(token tkn){
    if(tkn.ctgr == PR &&
       (tkn.Cod == caracter ||
        tkn.Cod == inteiro ||
        tkn.Cod == real ||
        tkn.Cod == booleano)) return 1;
    else return 0;
}

int isSemRetorno(token tkn){
    if(tkn.ctgr == PR &&
       (tkn.Cod == semretorno)) return 1;
    else return 0;
}

int isID(token tkn){
    if(tkn.ctgr == ID) return 1;
    else return 0;
}

int isOPRel(token tkn){
    if(tkn.ctgr == SN &&
       (tkn.Cod == igual ||
        tkn.Cod == diferente ||
        tkn.Cod == menorigual ||
        tkn.Cod == menor ||
        tkn.Cod == maiorigual ||
        tkn.Cod == maior)) return 1;
    else return 0;
}

int checkToken(token tkn,category cat,int cod){
    if(tkn.ctgr == cat && tkn.Cod == cod) return 1;
    else return 0;
}

int isTerminalFactor(token tkn){
    if(tkn.ctgr == CT_I || tkn.ctgr == CT_R || tkn.ctgr == CT_C || tkn.ctgr == LT) return 1;
    else return 0;
}

//=========================================================
//-------------------   ASDR CODE   -----------------------
//=========================================================

//PROG
void prog(){
    returnTrigger = 0;
    returnType = 0;
    funcReturnType = 0;
    parseTokens();
    if(isTipo(currentTkn)){
        funcReturnType = currentTkn.Cod;
        currentType = currentTkn.Cod;
        parseTokens();
        if(isID(currentTkn)){
            //Before storing the symbol, checking if it is function or variable
            strcpy(currentLexema, currentTkn.lexema);
            parseTokens();
            if(checkToken(currentTkn,SN,virgula)){
                multVarCont++;
                storeSymbol(tblSymbolsCurrentIndex,currentLexema,currentType,GLOBAL,VARIABLE,FALSE);
                if(checkSymbolConflict(currentLexema,GLOBAL,VARIABLE)){
                    semanticError(CONFLICTING_VARIABLE_NAME);
                }
                tblSymbolsCurrentIndex++;
                //Checking for ',' again just to keep code structure
                while(checkToken(currentTkn,SN,virgula)){
                    parseTokens();
                    if(isID(currentTkn)){
                        multVarCont++;
                        //Using currentLexema temp variable here just to keep code structure
                        strcpy(currentLexema, currentTkn.lexema);
                        storeSymbol(tblSymbolsCurrentIndex,currentLexema,currentType,GLOBAL,VARIABLE,FALSE);
                        if(checkSymbolConflict(currentLexema,GLOBAL,VARIABLE)){
                            semanticError(CONFLICTING_VARIABLE_NAME);
                        }
                        tblSymbolsCurrentIndex++;
                        parseTokens();
                    }else syntacticError(ID_AFTER_COMMA_EXPECTED);
                }
                if(checkToken(currentTkn,SN,pontovirgula)){
                    sprintf(tmpStr,"AMEM %d",multVarCont);
                    codeGen(tmpStr);
                    multVarCont = 0;
                    prog();
                }else syntacticError(MISSING_DECL_PONCTUATION);
            }else if(checkToken(currentTkn,SN,abre_parenteses)){
                if(strcmp(currentLexema,"principal") == 0){
                    mainFunction = 1;
                }
                storeSymbol(tblSymbolsCurrentIndex,currentLexema,currentType,GLOBAL,FUNCTION,FALSE);
                if(checkSymbolConflict(currentLexema,GLOBAL,FUNCTION)){
                    semanticError(CONFLICTING_FUNCTION_NAME);
                }
                tblSymbolsCurrentIndex++;
                strcpy(funcTempLexema, currentLexema);
                funcTempType = currentType;

                generateLabel();
                strcpy(FL,lastLabel);
                sprintf(tmpStr,"LABEL %s",FL);
                codeGen(tmpStr);
                codeGen("INIPR 1");
                alignFuncLabel(funcTempLexema,FL);

                tipos_param();
                comparePrototypeFunction(funcTempLexema, funcTempType);
                parseTokens();
                if(checkToken(currentTkn,SN,fecha_parenteses)){
                    parseTokens();
                    if(checkToken(currentTkn,SN,abre_chave)){
                        //Interior of a Function.
                        //Loop for declaration of variables.
                        currentFuncVarCont = 0;
                        while(isTipo(nextTkn)){
                            parseTokens();
                            currentType = currentTkn.Cod;
                            parseTokens();
                            multVarCont = 0;
                            if(isID(currentTkn)){
                                multVarCont++;
                                currentFuncVarCont++;
                                storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,VARIABLE,FALSE);
                                if(checkSymbolConflict(currentTkn.lexema,LOCAL,VARIABLE)){
                                    semanticError(CONFLICTING_VARIABLE_NAME);
                                }
                                tblSymbolsCurrentIndex++;
                                while(checkToken(nextTkn,SN,virgula)){
                                    parseTokens();
                                    parseTokens();
                                    if(isID(currentTkn)){
                                        multVarCont++;
                                        currentFuncVarCont++;
                                        storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,VARIABLE,FALSE);
                                        if(checkSymbolConflict(currentTkn.lexema,LOCAL,VARIABLE)){
                                            semanticError(CONFLICTING_VARIABLE_NAME);
                                        }
                                        tblSymbolsCurrentIndex++;
                                        //End of Interaction
                                    }else syntacticError(ID_AFTER_COMMA_EXPECTED);
                                }
                                sprintf(tmpStr,"AMEM %d",multVarCont);
                                codeGen(tmpStr);
                            }else syntacticError(ID_AFTER_TYPE_EXPECTED);
                            parseTokens();
                            if(checkToken(currentTkn,SN,pontovirgula)){
                                //End of Interaction
                            }else syntacticError(SEMICOLON_MISSING);
                        }
                        cmd();
                        //Token is parsed by CMD, if it is not the correct one, an
                        //error will trigger.
                        if(checkToken(currentTkn,SN,fecha_chave)){
                            if(!returnTrigger){
                                semanticError(MISSING_RETURN);
                            }else if(returnType == 0){
                                semanticError(WRONG_RETURN_TYPE);
                            }
                            zombifyLocalParameters();
                            destroyLocalVariables();
                            if(currentFuncVarCont > 0){
                                sprintf(tmpStr,"DMEM %d",currentFuncVarCont);
                                codeGen(tmpStr);
                            }
                            codeGen("RET 1,0");
                            //End of a function.
                            //Calling PROG again.
                            prog();
                        }else syntacticError(UNEXPECTED_SYMBOL);
                    }else syntacticError(OPEN_CURLY_BRACKETS_EXPECTED);
                }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
            }else if(checkToken(currentTkn,SN,pontovirgula)){
                storeSymbol(tblSymbolsCurrentIndex,currentLexema,currentType,GLOBAL,VARIABLE,FALSE);
                if(checkSymbolConflict(currentLexema,GLOBAL,VARIABLE)){
                    semanticError(CONFLICTING_VARIABLE_NAME);
                }
                tblSymbolsCurrentIndex++;
                codeGen("AMEM 1");
                //End of variable declaration.
                //Calling PROG again.
                prog();
            }else syntacticError(MISSING_DECL_PONCTUATION);
        }else syntacticError(ID_AFTER_TYPE_EXPECTED);
    }else if(checkToken(currentTkn,PR,semretorno)){
        funcReturnType = currentTkn.Cod;
        currentType = currentTkn.Cod;
        funcTempType = currentTkn.Cod;
        parseTokens();
        if(isID(currentTkn)){
            if(strcmp(currentTkn.lexema,"principal") == 0){
                mainFunction = 1;
            }
            storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,GLOBAL,FUNCTION,FALSE);
            if(checkSymbolConflict(currentTkn.lexema,GLOBAL,FUNCTION)){
                semanticError(CONFLICTING_FUNCTION_NAME);
            }
            tblSymbolsCurrentIndex++;
            strcpy(funcTempLexema, currentTkn.lexema);
            parseTokens();
            if(checkToken(currentTkn,SN,abre_parenteses)){
                generateLabel();
                strcpy(FL,lastLabel);
                sprintf(tmpStr,"LABEL %s",FL);
                codeGen(tmpStr);
                codeGen("INIPR 1");
                alignFuncLabel(funcTempLexema,FL);

                tipos_param();
                comparePrototypeFunction(funcTempLexema, funcTempType);
                parseTokens();
                if(checkToken(currentTkn,SN,fecha_parenteses)){
                    parseTokens();
                    if(checkToken(currentTkn,SN,abre_chave)){
                        //Interior of a Function.
                        //Loop for declaration of variables.
                        currentFuncVarCont = 0;
                        while(isTipo(nextTkn)){
                            parseTokens();
                            currentType = currentTkn.Cod;
                            multVarCont = 0;
                            parseTokens();
                            if(isID(currentTkn)){
                                multVarCont++;
                                currentFuncVarCont++;
                                storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,VARIABLE,FALSE);
                                if(checkSymbolConflict(currentTkn.lexema,LOCAL,VARIABLE)){
                                    semanticError(CONFLICTING_VARIABLE_NAME);
                                }
                                tblSymbolsCurrentIndex++;
                                while(checkToken(nextTkn,SN,virgula)){
                                    parseTokens();
                                    parseTokens();
                                    if(isID(currentTkn)){
                                        multVarCont++;
                                        currentFuncVarCont++;
                                        storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,VARIABLE,FALSE);
                                        if(checkSymbolConflict(currentTkn.lexema,LOCAL,VARIABLE)){
                                            semanticError(CONFLICTING_VARIABLE_NAME);
                                        }
                                        tblSymbolsCurrentIndex++;
                                        //End of Interaction
                                    }else syntacticError(ID_AFTER_COMMA_EXPECTED);
                                }
                                sprintf(tmpStr,"AMEM %d",multVarCont);
                                codeGen(tmpStr);
                            }else syntacticError(ID_AFTER_TYPE_EXPECTED);
                            parseTokens();
                            if(checkToken(currentTkn,SN,pontovirgula)){
                                //End of Interaction
                            }else syntacticError(SEMICOLON_MISSING);
                        }
                        cmd();
                        //Token is parsed by CMD, if it is not the correct one, an
                        //error will trigger.
                        if(checkToken(currentTkn,SN,fecha_chave)){
                            if(returnTrigger){
                                semanticError(RETURN_IN_VOID_FUNCTION);
                            }
                            zombifyLocalParameters();
                            destroyLocalVariables();
                            if(currentFuncVarCont > 0){
                                sprintf(tmpStr,"DMEM %d",currentFuncVarCont);
                                codeGen(tmpStr);
                            }
                            codeGen("RET 1,0");
                            //End of a function.
                            //Calling PROG again.
                            prog();
                        }else syntacticError(UNEXPECTED_SYMBOL);
                    }else syntacticError(OPEN_CURLY_BRACKETS_EXPECTED);
                }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
            }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
        }else syntacticError(ID_AFTER_TYPE_EXPECTED);
    }else if(checkToken(currentTkn,PR,prototipo)){
        parseTokens();
        if(isTipo(currentTkn) || checkToken(currentTkn,PR,semretorno)){
            currentType = currentTkn.Cod;
            parseTokens();
            if(isID(currentTkn)){
                storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,GLOBAL,PROTOTYPE,FALSE);
                if(checkSymbolConflict(currentTkn.lexema,GLOBAL,PROTOTYPE)){
                    semanticError(FUNCTION_BEFORE_PROTOTYPE);
                }
                tblSymbolsCurrentIndex++;
                parseTokens();
                if(checkToken(currentTkn,SN,abre_parenteses)){
                    tipos_p_opc();
                    parseTokens();
                    if(checkToken(currentTkn,SN,fecha_parenteses)){
                        zombifyLocalParameters();
                        //Lookup for ','
                        while(checkToken(nextTkn,SN,virgula)){
                            parseTokens();
                            parseTokens();
                            if(isID(currentTkn)){
                                storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,GLOBAL,PROTOTYPE,FALSE);
                                if(checkSymbolConflict(currentTkn.lexema,GLOBAL,PROTOTYPE)){
                                    semanticError(CONFLICTING_FUNCTION_NAME);
                                }
                                tblSymbolsCurrentIndex++;
                                parseTokens();
                                if(checkToken(currentTkn,SN,abre_parenteses)){
                                    tipos_p_opc();
                                    parseTokens();
                                    if(checkToken(currentTkn,SN,fecha_parenteses)){
                                        zombifyLocalParameters();
                                        //End of interaction
                                    }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
                                }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
                            }else syntacticError(ID_AFTER_COMMA_EXPECTED);
                        }
                        parseTokens();
                        if(checkToken(currentTkn,SN,pontovirgula)){
                            //End of Prototypes declaration.
                            //Calling PROG again.
                            prog();
                        }else syntacticError(SEMICOLON_MISSING);
                    }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
                }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
            }else syntacticError(ID_AFTER_TYPE_EXPECTED);
        }else syntacticError(PROTOTYPE_TYPE_EXPECTED);
    }else if(checkToken(currentTkn,ENDFILE,fileend)){
        if(!mainFunction){
            semanticError(MAIN_FUNCTION_NOT_FOUND);
        }
        //End of File and syntactic analysis
    }else syntacticError(WRONG_START_STATEMENT);
}

//TIPOS_PARAM
void tipos_param(){
    parseTokens();
    if(isTipo(currentTkn)){
        currentType = currentTkn.Cod;
        parseTokens();
        if(isID(currentTkn)){
            storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,PARAMETER,FALSE);
            if(checkSymbolConflict(currentTkn.lexema,LOCAL,PARAMETER)){
                semanticError(CONFLICTING_PARAMETER_NAME);
            }
            tblSymbolsCurrentIndex++;
            //Lookup for ','
            while(checkToken(nextTkn,SN,virgula)){
                parseTokens();
                parseTokens();
                if(isTipo(currentTkn)){
                    currentType = currentTkn.Cod;
                    parseTokens();
                    if(isID(currentTkn)){
                        storeSymbol(tblSymbolsCurrentIndex,currentTkn.lexema,currentType,LOCAL,PARAMETER,FALSE);
                        if(checkSymbolConflict(currentTkn.lexema,LOCAL,PARAMETER)){
                            semanticError(CONFLICTING_PARAMETER_NAME);
                        }
                        tblSymbolsCurrentIndex++;
                        //End of Interaction
                    }else syntacticError(ID_AFTER_TYPE_EXPECTED);
                }else syntacticError(PARAM_TYPE_EXPECTED);
            }
            //Exiting
        }else syntacticError(ID_AFTER_TYPE_EXPECTED);
    }else if(checkToken(currentTkn,PR,semparam)){
        //Exiting
    }else syntacticError(PARAM_TYPE_EXPECTED);
}

//TIPOS_P_OPC
void tipos_p_opc(){
    parseTokens();
    if(isTipo(currentTkn)){
        currentType = currentTkn.Cod;
        //Lookup for ID
        if(isID(nextTkn)){
            parseTokens();
            strcpy(tblSymbols[tblSymbolsCurrentIndex].lexema, currentTkn.lexema);
        }else{
            strcpy(tblSymbols[tblSymbolsCurrentIndex].lexema, "_NULL");
        }
        storeSymbol(tblSymbolsCurrentIndex,"_NULL",currentType,LOCAL,PARAMETER,FALSE);
        tblSymbolsCurrentIndex++;
        while(checkToken(nextTkn,SN,virgula)){
            parseTokens();
            parseTokens();
            if(isTipo(currentTkn)){
                currentType = currentTkn.Cod;
                //Lookup for ID
                if(isID(nextTkn)){
                    parseTokens();
                    strcpy(tblSymbols[tblSymbolsCurrentIndex].lexema, currentTkn.lexema);
                }else{
                    strcpy(tblSymbols[tblSymbolsCurrentIndex].lexema, "_NULL");
                }
                storeSymbol(tblSymbolsCurrentIndex,"_NULL",currentType,LOCAL,PARAMETER,FALSE);
                if(checkSymbolConflict(currentTkn.lexema,LOCAL,PARAMETER)){
                    semanticError(CONFLICTING_PARAMETER_NAME);
                }
                tblSymbolsCurrentIndex++;
                //End of Interaction
            }else syntacticError(TYPE_EXPECTED);
        }
        //Exiting
    }else if(checkToken(currentTkn,PR,semparam)){
        //Exiting
    }else syntacticError(PARAM_TYPE_EXPECTED);
}

//CMD
void cmd(){
    int tempResult, atribResult;
    char Lx[10];
    char Ly[10];
    char Lz[10];
    char Lw[10];

    parseTokens();
    if(checkToken(currentTkn,PR,se)){
        parseTokens();
        if(checkToken(currentTkn,SN,abre_parenteses)){
            tempResult = expr();
            generateLabel();
            strcpy(Lx,lastLabel);
            generateLabel();
            strcpy(Ly,lastLabel);
        //    sprintf(tmpStr,"LABEL %s",Lx);
        //    codeGen(tmpStr);
            if(tempResult != booleano && tempResult != inteiro){
                semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            parseTokens();
            if(checkToken(currentTkn,SN,fecha_parenteses)){
                sprintf(tmpStr,"GOFALSE %s",Lx);
                codeGen(tmpStr);
                //Calling CMD's inside the SE(if) statement.
                cmd();
                //Token is parsed by CMD.
                if(checkToken(currentTkn,PR,senao)){
                    sprintf(tmpStr,"GOTO %s",Ly);
                    codeGen(tmpStr);
                    sprintf(tmpStr,"LABEL %s",Lx);
                    codeGen(tmpStr);
                    //Calling CMD's inside the SENAO(else) statement.
                    cmd();
                    sprintf(tmpStr,"LABEL %s",Ly);
                    codeGen(tmpStr);
                }else{
                    sprintf(tmpStr,"LABEL %s",Lx);
                    codeGen(tmpStr);
                }
            }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
        }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
    }else if(checkToken(currentTkn,PR,enquanto)){
        generateLabel();
        strcpy(Lx,lastLabel);
        generateLabel();
        strcpy(Ly,lastLabel);

        sprintf(tmpStr,"LABEL %s",Lx);
        codeGen(tmpStr);

        parseTokens();
        if(checkToken(currentTkn,SN,abre_parenteses)){
            tempResult = expr();
            if(tempResult != booleano && tempResult != inteiro){
                semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            parseTokens();
            if(checkToken(currentTkn,SN,fecha_parenteses)){
                sprintf(tmpStr,"GOFALSE %s",Ly);
                codeGen(tmpStr);
                //End of this execution of CMD.
                //Calling the next CMD, inside the ENQUATO(while) statement.
                cmd();
                sprintf(tmpStr,"GOTO %s",Lx);
                codeGen(tmpStr);
                sprintf(tmpStr,"LABEL %s",Ly);
                codeGen(tmpStr);
            }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
        }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
    }else if(checkToken(currentTkn,PR,para)){

        generateLabel();
        strcpy(Lx,lastLabel);
        generateLabel();
        strcpy(Ly,lastLabel);
        generateLabel();
        strcpy(Lz,lastLabel);
        generateLabel();
        strcpy(Lw,lastLabel);

        parseTokens();
        if(checkToken(currentTkn,SN,abre_parenteses)){
            if(isID(nextTkn)){
                parseTokens();
                atrib();
            }
            parseTokens();
            if(checkToken(currentTkn,SN,pontovirgula)){
                //Lookup for terminator ';'. If not, calls EXPR
                if(checkToken(nextTkn,SN,pontovirgula)){
                    //Continues executing bellow.
                }else{
                    sprintf(tmpStr,"LABEL %s",Lw);
                    codeGen(tmpStr);

                    tempResult = expr();
                    if(tempResult != booleano && tempResult != inteiro){
                        semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    }
                    sprintf(tmpStr,"GOFALSE %s",Lx);
                    codeGen(tmpStr);
                    sprintf(tmpStr,"GOTO %s",Ly);
                    codeGen(tmpStr);
                }
                parseTokens();
                //Checking again for ';' just to keep code structure
                if(checkToken(currentTkn,SN,pontovirgula)){
                    sprintf(tmpStr,"LABEL %s",Lz);
                    codeGen(tmpStr);

                    if(isID(nextTkn)){
                        parseTokens();
                        atrib();
                    }

                    sprintf(tmpStr,"GOTO %s",Lw);
                    codeGen(tmpStr);

                    parseTokens();
                    if(checkToken(currentTkn,SN,fecha_parenteses)){
                        sprintf(tmpStr,"LABEL %s",Ly);
                        codeGen(tmpStr);
                        //End of this execution of CMD.
                        //Calling the next CMD, inside the PARA(for) statement.
                        cmd();
                        sprintf(tmpStr,"GOTO %s",Lz);
                        codeGen(tmpStr);
                        sprintf(tmpStr,"LABEL %s",Lx);
                        codeGen(tmpStr);
                    }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
                }else syntacticError(SEMICOLON_MISSING);
            }else syntacticError(SEMICOLON_MISSING);
        }else syntacticError(OPEN_PARENTHESIS_EXPECTED);
    }else if(checkToken(currentTkn,PR,retorne)){
        returnTrigger = 1;
        //Lookup for terminator ';'. If not, calls EXPR
        if(checkToken(nextTkn,SN,pontovirgula)){
            returnType = 0;
            //Continues executing bellow.
        }else{
            returnType = expr();
            if(funcReturnType){
                if(funcReturnType != returnType){
                    semanticError(WRONG_RETURN_TYPE);
                }
            }else semanticError(RETURN_IN_VOID_FUNCTION);
        }
        parseTokens();
        if(checkToken(currentTkn,SN,pontovirgula)){
            //End of this execution of CMD.
            //Calling it again.
            if(currentFuncVarCont > 0){
                sprintf(tmpStr,"DMEM %d",currentFuncVarCont);
                codeGen(tmpStr);
            }
            codeGen("RET 1,0");
            cmd();
        }else syntacticError(SEMICOLON_MISSING);
    }else if(isID(currentTkn)){
        strcpy(currentLexema,currentTkn.lexema);
        strcpy(tmpLex,currentTkn.lexema);
        //Lookup for '=' or '('
        if(checkToken(nextTkn,SN,atribuicao)){
            tempResult = checkVariablePreviousDeclaration(currentLexema);
            if(tempResult == -1){
                semanticError(VARIABLE_NOT_DECLARED);
            }
            atribResult = atrib();
            switch(tempResult){
                case caracter:
                    if(atribResult != caracter && atribResult != inteiro){
                        semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    }
                    break;
                case inteiro:
                    if(atribResult != caracter && atribResult != inteiro && atribResult != booleano){
                        semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    }
                    break;
                case real:
                    if(atribResult != real){
                        semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    }
                    break;
                case booleano:
                    if(atribResult != inteiro && atribResult != booleano){
                        semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    }
                    break;
                default: semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            getVarRegMem(tmpLex);
            sprintf(tmpStr,"STOR %d,%d",currentRegister,currentRelativeAddress);
            currentRelativeAddress++;
            codeGen(tmpStr);
            parseTokens();
            if(checkToken(currentTkn,SN,pontovirgula)){
                //End of this execution of CMD.
                //Calling it again.
                cmd();
            }else syntacticError(SEMICOLON_MISSING);
        }else if (checkToken(nextTkn,SN,abre_parenteses)){
            tempResult = checkFunctionPreviousDeclaration(currentLexema);
            if(tempResult == -1){
                semanticError(FUNCTION_NOT_DECLARED);
            }else if(tempResult >= 1){
                semanticError(VOID_FUNCTION_EXPECTED);
            }
            setFunctionParametersCheckPointers(currentLexema);

            if(tempResult != semretorno){
                codeGen("AMEM 1");
            }
            getFuncLabel(currentLexema);

            parseTokens();
            //Lookup for terminator ')'. If not, calls EXPR
            if(checkToken(nextTkn,SN,fecha_parenteses)){
                sprintf(tmpStr,"CALL %s",FL);
                codeGen(tmpStr);
                //Continues executing bellow.
            }else{
                tempResult = expr();
                compareParameters(tempResult);
                //Multiple EXPR's
                while(checkToken(nextTkn,SN,virgula)){
                    parseTokens();
                    tempResult = expr();
                    compareParameters(tempResult);
                }
            }
            parseTokens();
            if(checkToken(currentTkn,SN,fecha_parenteses)){
                sprintf(tmpStr,"CALL %s",FL);
                codeGen(tmpStr);
                parseTokens();
                if(checkToken(currentTkn,SN,pontovirgula)){
                    //End of this execution of CMD.
                    //Calling it again.
                    cmd();
                }else syntacticError(SEMICOLON_MISSING);
            }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
        }else syntacticError(UNEXPECTED_SYMBOL);
    }else if(checkToken(currentTkn,SN,abre_chave)){
        cmd();
        if(checkToken(currentTkn,SN,fecha_chave)){
            //End of this execution of CMD.
            //Calling it again.
            cmd();
        }else syntacticError(CLOSE_CURLY_BRACKETS_EXPECTED);
    }else if(checkToken(currentTkn,SN,pontovirgula)){
        //End of this execution of CMD.
        //Calling it again.
        cmd();
    }else ;
}

//ATRIB
//Requires a parse before calling it
int atrib(){
    int typeResult;

    if(isID(currentTkn)){
        parseTokens();
        if(checkToken(currentTkn,SN,atribuicao)){
            typeResult = expr();
        }else syntacticError(ATRIB_EXPECTED);
    }else syntacticError(UNEXPECTED_SYMBOL);
    return typeResult;
}

//EXPR
int expr(){
    int typeResult, tempResult, OP;
    typeResult = expr_simples();
    char Lx[10];
    char Ly[10];
    char Lz[10];
    //Lookup for OP_Rel
    if(isOPRel(nextTkn)){
        parseTokens();
        OP = currentTkn.Cod;
        tempResult = expr_simples();
        if(tempResult != real && typeResult != real){
            typeResult = booleano;
        }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);

        generateLabel();
        strcpy(Lx,lastLabel);
        generateLabel();
        strcpy(Ly,lastLabel);
        generateLabel();
        strcpy(Lz,lastLabel);

        if(OP == igual){
            codeGen("SUB");
            sprintf(tmpStr,"GOFALSE %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 0");
            sprintf(tmpStr,"GOTO %s",Ly);
            codeGen(tmpStr);
            sprintf(tmpStr,"LABEL %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 1");
            sprintf(tmpStr,"LABEL %s",Ly);
            codeGen(tmpStr);
        }else if(OP == diferente || OP == maior){
            codeGen("SUB");
            sprintf(tmpStr,"GOTRUE %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 0");
            sprintf(tmpStr,"GOTO %s",Ly);
            codeGen(tmpStr);
            sprintf(tmpStr,"LABEL %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 1");
            sprintf(tmpStr,"LABEL %s",Ly);
            codeGen(tmpStr);
        }else if(OP == menor){
            codeGen("SUB");
            codeGen("COPY");
            sprintf(tmpStr,"GOFALSE %s",Lx);
            codeGen(tmpStr);
            sprintf(tmpStr,"GOTRUE %s",Ly);
            codeGen(tmpStr);
            codeGen("PUSH 1");
            sprintf(tmpStr,"GOTO %s",Lz);
            codeGen(tmpStr);
            sprintf(tmpStr,"LABEL %s",Lx);
            codeGen(tmpStr);
            codeGen("POP");
            sprintf(tmpStr,"LABEL %s",Ly);
            codeGen(tmpStr);
            codeGen("PUSH 0");
            sprintf(tmpStr,"LABEL %s",Lz);
            codeGen(tmpStr);
        }else if(OP == maiorigual){
            codeGen("SUB");
            codeGen("COPY");
            sprintf(tmpStr,"GOFALSE %s",Lx);
            codeGen(tmpStr);
            sprintf(tmpStr,"GOTRUE %s",Ly);
            codeGen(tmpStr);
            codeGen("PUSH 0");
            sprintf(tmpStr,"GOTO %s",Lz);
            codeGen(tmpStr);
            sprintf(tmpStr,"LABEL %s",Lx);
            codeGen(tmpStr);
            codeGen("POP");
            sprintf(tmpStr,"LABEL %s",Ly);
            codeGen(tmpStr);
            codeGen("PUSH 1");
            sprintf(tmpStr,"LABEL %s",Lz);
            codeGen(tmpStr);
        }else if(OP == menorigual){
            codeGen("SUB");
            sprintf(tmpStr,"GOTRUE %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 1");
            sprintf(tmpStr,"GOTO %s",Ly);
            codeGen(tmpStr);
            sprintf(tmpStr,"LABEL %s",Lx);
            codeGen(tmpStr);
            codeGen("PUSH 0");
            sprintf(tmpStr,"LABEL %s",Ly);
            codeGen(tmpStr);
        }
    }
    return typeResult;
}

//EXPR_SIMPLES
int expr_simples(){
    int typeResult, tempResult, OP;
    char flabel[10];
    int orTrigger = 0;

    //Lookup for '+' or '-'
    if(checkToken(nextTkn,SN,soma) || checkToken(nextTkn,SN,subtracao)){
        parseTokens();
        typeResult = termo();
        if(typeResult == caracter){
            typeResult = inteiro;
        }else if(typeResult == booleano){
            semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
        }
    }else typeResult = termo();
    //Lookup for '+', '-' or '||'
    while(checkToken(nextTkn,SN,soma) || checkToken(nextTkn,SN,subtracao) || checkToken(nextTkn,SN,or)){
        parseTokens();
        if(currentTkn.Cod == soma || currentTkn.Cod == subtracao){
            OP = currentTkn.Cod;
            tempResult = termo();
            switch(typeResult){
                case caracter:
                    if(tempResult == caracter || tempResult == inteiro){
                        typeResult = inteiro;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case inteiro:
                    if(tempResult == caracter || tempResult == inteiro){
                        typeResult = inteiro;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case real:
                    if(tempResult == real){
                        typeResult = real;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case booleano:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                default: semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            if(typeResult == real){
                if(OP == soma){
                    codeGen("ADDF");
                }else if(OP == subtracao){
                    codeGen("SUBF");
                }
            }else{
                if(OP == soma){
                    codeGen("ADD");
                }else if(OP == subtracao){
                    codeGen("SUB");
                }
            }
        }else if(currentTkn.Cod == or){
            if(orTrigger){
                codeGen("POP");
            }
            orTrigger = 1;
            generateLabel();
            strcpy(flabel,lastLabel);

            codeGen("COPY");
            sprintf(tmpStr,"GOTRUE %s",flabel);
            codeGen(tmpStr);
            codeGen("POP");


            tempResult = termo();
            switch(typeResult){
                case caracter:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case inteiro:
                    if(tempResult == inteiro || tempResult == booleano){
                        typeResult = booleano;
                    }
                    break;
                case real:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case booleano:
                    if(tempResult == inteiro || tempResult == booleano){
                        typeResult = booleano;
                    }
                    break;
                default: semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            codeGen("COPY");
            sprintf(tmpStr,"GOTRUE %s",flabel);
            codeGen(tmpStr);
        }
    }
    if(orTrigger){
        sprintf(tmpStr,"LABEL %s",flabel);
        codeGen(tmpStr);
    }
    return typeResult;
}

//TERMO
int termo(){
    int typeResult, tempResult, OP;
    char flabel[10];
    int andTrigger = 0;

    typeResult = fator();
    //Lookup for '*', '/' or '&&'
    while(checkToken(nextTkn,SN,multiplicacao) || checkToken(nextTkn,SN,divisao) || checkToken(nextTkn,SN,and)){
        parseTokens();
        if(currentTkn.Cod == multiplicacao || currentTkn.Cod == divisao){
            OP = currentTkn.Cod;
            tempResult = fator();
            switch(typeResult){
                case caracter:
                    if(tempResult == caracter || tempResult == inteiro){
                        typeResult = inteiro;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case inteiro:
                    if(tempResult == caracter || tempResult == inteiro){
                        typeResult = inteiro;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case real:
                    if(tempResult == real){
                        typeResult = real;
                    }else semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case booleano:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                default: semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            if(typeResult == real){
                if(OP == multiplicacao){
                    codeGen("MULF");
                }else if(OP == divisao){
                    codeGen("DIVF");
                }
            }else{
                if(OP == multiplicacao){
                    codeGen("MUL");
                }else if(OP == divisao){
                    codeGen("DIV");
                }
            }
        }else if(currentTkn.Cod == and){
            if(andTrigger){
                codeGen("POP");
            }
            andTrigger = 1;
            generateLabel();
            strcpy(flabel,lastLabel);

            codeGen("COPY");
            sprintf(tmpStr,"GOFALSE %s",flabel);
            codeGen(tmpStr);
            codeGen("POP");

            tempResult = fator();
            switch(typeResult){
                case caracter:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case inteiro:
                    if(tempResult == inteiro || tempResult == booleano){
                        typeResult = booleano;
                    }
                    break;
                case real:
                    semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
                    break;
                case booleano:
                    if(tempResult == inteiro || tempResult == booleano){
                        typeResult = booleano;
                    }
                    break;
                default: semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
            }
            codeGen("COPY");
            sprintf(tmpStr,"GOFALSE %s",flabel);
            codeGen(tmpStr);
        }
    }
    if(andTrigger){
        sprintf(tmpStr,"LABEL %s",flabel);
        codeGen(tmpStr);
    }
    return typeResult;
}

//FATOR
int fator(){
    int typeResult, tempResult;
    char Lx[10];
    char Ly[10];

    parseTokens();
    if(isTerminalFactor(currentTkn)){
        if(currentTkn.ctgr == CT_I){
            typeResult = inteiro;
            sprintf(tmpStr,"PUSH %d",currentTkn.valueInt);
            codeGen(tmpStr);
        }else if(currentTkn.ctgr == CT_R){
            typeResult = real;
            sprintf(tmpStr,"PUSH %f",currentTkn.valueReal);
            codeGen(tmpStr);
        }else if(currentTkn.ctgr == CT_C){
            typeResult = caracter;
            sprintf(tmpStr,"PUSH %s",tblCharacters[tblCharactersCurrentIndex-1]);
            codeGen(tmpStr);
        }else if(currentTkn.ctgr == LT){
            typeResult = caracter;
            sprintf(tmpStr,"PUSH %s",tblLiterals[tblLiteralsCurrentIndex-1]);
            codeGen(tmpStr);
        }else syntacticError(UNEXPECTED_SYMBOL);
        //Exiting
    }else if(isID(currentTkn)){
        strcpy(currentLexema,currentTkn.lexema);
        if(checkToken(nextTkn,SN,abre_parenteses)){
            typeResult = checkFunctionPreviousDeclaration(currentLexema);
            if(typeResult != semretorno){
                codeGen("AMEM 1");
            }
            getFuncLabel(currentLexema);
      //      sprintf(tmpStr,"CALL %s",FL);
       //     codeGen(tmpStr);

            if(typeResult == -1){
                semanticError(FUNCTION_NOT_DECLARED);
            }else if(typeResult == 0){
                semanticError(NO_VOID_FUNCTION_EXPECTED);
            }
            setFunctionParametersCheckPointers(currentLexema);
            parseTokens();
            if(checkToken(nextTkn,SN,fecha_parenteses)){
                parseTokens();
                sprintf(tmpStr,"CALL %s",FL);
                codeGen(tmpStr);
                //Exiting
            }else{
                tempResult = expr();
                compareParameters(tempResult);
                while(checkToken(nextTkn,SN,virgula)){
                    parseTokens();
                    tempResult = expr();
                    compareParameters(tempResult);
                }
                parseTokens();
                if(checkToken(currentTkn,SN,fecha_parenteses)){
                    sprintf(tmpStr,"CALL %s",FL);
                    codeGen(tmpStr);
                    //Exiting
                }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
            }
        }else{
            typeResult = checkVariablePreviousDeclaration(currentLexema);
            if(typeResult == -1){
                semanticError(VARIABLE_NOT_DECLARED);
            }
            getVarRegMem(currentTkn.lexema);
            sprintf(tmpStr,"LOAD %d,%d",currentRegister,currentRelativeAddress);
            codeGen(tmpStr);
            //Exiting after variable or parameter
        }
    }else if(checkToken(currentTkn,SN,abre_parenteses)){
        typeResult = expr();
        parseTokens();
        if(checkToken(currentTkn,SN,fecha_parenteses)){
            //Exiting
        }else syntacticError(CLOSE_PARENTHESIS_EXPECTED);
    }else if(checkToken(currentTkn,SN,not)){
        typeResult = fator();
        if (typeResult != inteiro && typeResult != booleano){
            semanticError(INCOMPATIBLE_EXPRESSION_TYPES);
        }
        typeResult = booleano;

        generateLabel();
        strcpy(Lx,lastLabel);
        generateLabel();
        strcpy(Ly,lastLabel);

        sprintf(tmpStr,"GOFALSE %s",Lx);
        codeGen(tmpStr);
        codeGen("PUSH 0");
        sprintf(tmpStr,"GOTO %s",Ly);
        codeGen(tmpStr);
        sprintf(tmpStr,"LABEL %s",Lx);
        codeGen(tmpStr);
        codeGen("PUSH 1");
        sprintf(tmpStr,"LABEL %s",Ly);
        codeGen(tmpStr);
    }else syntacticError(FACTOR_EXPECTED);
    return typeResult;
}
