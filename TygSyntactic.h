/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <TygSemantic.h>
#include <TygCodeGen.h>

enum tblSyntacticCodErrors{
    WRONG_START_STATEMENT,
    ID_AFTER_TYPE_EXPECTED,
    MISSING_DECL_PONCTUATION,
    ID_AFTER_COMMA_EXPECTED,
    PROTOTYPE_TYPE_EXPECTED,
    OPEN_PARENTHESIS_EXPECTED,
    CLOSE_PARENTHESIS_EXPECTED,
    OPEN_CURLY_BRACKETS_EXPECTED,
    CLOSE_CURLY_BRACKETS_EXPECTED,
    SEMICOLON_MISSING,
    PARAM_TYPE_EXPECTED,
    TYPE_EXPECTED,
    UNEXPECTED_SYMBOL,
    FACTOR_EXPECTED,
    ATRIB_EXPECTED
};

//Function Prototypes
void activateTygCompiler();
void syntacticError(int errorCode);
int isTipo(token tkn);
int isSemRetorno(token tkn);
int isID(token tkn);
int isOPRel(token tkn);
int checkToken(token tkn,category cat,int cod);
int isTerminalFactor(token tkn);


void prog();
void tipos_param();
void tipos_p_opc();
void cmd();
int atrib();
int expr();
int expr_simples();
int termo();
int fator();
