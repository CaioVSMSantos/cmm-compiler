/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <TygAnalex.h>

#define tblSymbolsSize 1000

enum tblSemanticCodErrors{
    CONFLICTING_PARAMETER_NAME,
    CONFLICTING_FUNCTION_NAME,
    CONFLICTING_VARIABLE_NAME,
    FUNCTION_BEFORE_PROTOTYPE,
    PROT_FUNC_DIFFERENT_RETURN_TYPES,
    PROT_FUNC_DIFFERENT_PARAMETERS,
    PROT_FUNC_PARAM_DIFFERENT_LEXEME,
    PROT_FUNC_PARAM_DIFFERENT_TYPES,
    VARIABLE_NOT_DECLARED,
    FUNCTION_NOT_DECLARED,
    FUNCTION_CALL_WRONG_PARAMETERS,
    INCOMPATIBLE_EXPRESSION_TYPES,
    MISSING_RETURN,
    RETURN_IN_VOID_FUNCTION,
    WRONG_RETURN_TYPE,
    VOID_FUNCTION_EXPECTED,
    NO_VOID_FUNCTION_EXPECTED,
    MAIN_FUNCTION_NOT_FOUND
};

typedef enum {
    GLOBAL,
    LOCAL
} scope;

//Can't call 'category'
typedef enum {
    VARIABLE,
    FUNCTION,
    PARAMETER,
    PROTOTYPE
} group;

typedef enum{
    FALSE,
    TRUE
} zombie;

typedef struct {
    char lexema[50];
    int type;
    scope scp;
    group grp;
    zombie zmb;
} symbol;

symbol tblSymbols[tblSymbolsSize];
int tblSymbolsCurrentIndex;
int currentType;
char currentLexema[50];
char funcTempLexema[50];
int funcTempType;
int funcPointer;
int paramPointer;
int returnTrigger; //0 is false, 1 is true
int returnType;
int funcReturnType;
int mainFunction;

scope currentScope;
group currentGroup;

void initializeSemanticVariables();
void semanticError(int errorCod);
int checkSymbolConflict(char lexema[50], scope scp, group grp);
void storeSymbol(int idx, char lexema[50], int type, scope scp, group grp, zombie zmb);
void zombifyLocalParameters();
void destroyLocalVariables();
void comparePrototypeFunction(char lexema[50], int type);
int checkVariablePreviousDeclaration(char lexema[50]);
int checkFunctionPreviousDeclaration(char lexema[50]);
int setFunctionParametersCheckPointers(char lexema[50]);
void compareParameters(int type);

