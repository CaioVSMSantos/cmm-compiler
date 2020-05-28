/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define tblSignalsIndexSize 22
#define tblSignalsLength 4

#define tblReservedWordsIndexSize 13
#define tblReservedWordsLength 12

#define tblLiteralsIndexSize 200
#define tblLiteralsLength 200

#define tblCharactersIndexSize 200
#define tblCharactersLength 16

#define tblIdentifiersIndexSize 200
#define tblIdentifiersLength 50


//Tables
//Same codes used as "Type" of functions and parameters
enum tblCodReservedWords{semretorno,
                        caracter,
                        inteiro,
                        real,
                        booleano,
                        semparam,
                        se,
                        senao,
                        enquanto,
                        para,
                        retorne,
                        prototipo,
                        fileend};

enum tblCodSignals{soma,
                subtracao,
                multiplicacao,
                divisao,
                atribuicao,
                abre_parenteses,
                fecha_parenteses,
                abre_colchete,
                fecha_colchete,
                abre_chave,
                fecha_chave,
                not,
                and,
                or,
                igual,
                diferente,
                maior,
                maiorigual,
                menor,
                menorigual,
                pontovirgula,
                virgula
};

enum tblCodMessages{
    FILE_END,
    FILE_END_ERROR,
    REAL_NOT_FINISHED,
    INVALID_CHARCON
};

typedef enum {
    ID, PR, SN, CT_I, CT_R, CT_C, LT, ENDFILE
} category;

typedef struct {
    category ctgr;
    union {
        int Cod;
        int index;
        int valueInt;
        float valueReal;
        int valueBool;
        char lexema[50];
    };
} token;

char tblReservedWords[tblReservedWordsIndexSize][tblReservedWordsLength];
char tblSignals[tblSignalsIndexSize][tblSignalsLength];

char tblLiterals[tblLiteralsIndexSize][tblLiteralsLength];
char tblCharacters[tblCharactersIndexSize][tblCharactersLength];
char tblIdentifiers[tblIdentifiersIndexSize][tblIdentifiersLength];

//Variables
FILE *file;
int tblLiteralsCurrentIndex;
int tblCharactersCurrentIndex;
int tblIdentifiersCurrentIndex;
int automatonState;
int programLines ;
char c;
token currentTkn;
token nextTkn;

//Function Prototypes
void parseTokens();
void analexDisplayTokens();
void openFile();
void closeFile();
token getToken();
void printToken(token tkn);
void initializeAnalexVariables();
int isLetter(char c);
int isNumber(char c);
int isLetterNumberOrUnderline(char c);
void displayMessage(int msgCod);
void invalidCharacter(char c);
