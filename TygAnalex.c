/*---------------------------------------*/
//  Author: Caio Victor Sampaio
/*---------------------------------------*/
#include <TygAnalex.h>

void parseTokens(){
    currentTkn = nextTkn;
    nextTkn = getToken();
}

void analexDisplayTokens(){
    parseTokens();
    printf("\n  Extracting Tokens...\n\n");
    while(1){
        parseTokens();
        printToken(currentTkn);
        if(currentTkn.ctgr == ENDFILE){
            displayMessage(FILE_END);
        }
    }
}

void openFile() {
    file = NULL;
    char str1[50];
    char k[1];

    while(1){
        printf("  Please Input File Name With Extension(non-case sensitive):  ");
        scanf("%s", str1);

        if ((file = fopen(str1, "r")) == NULL) {
            printf("\n  Error! File not found! Input again?(Y/N) \n\n");
            fflush(stdin);
            scanf("%s", k);
            if(k[0] == 'n' || k[0] == 'N'){
                printf("Exiting Compiler...\n\n");
                exit(1);
                break;
            } else system("cls");
        } else {
            printf("\n  File Read Successful!\n\n");
            system("PAUSE");
            break;
        }
    }
}

void closeFile(){
    printf("\n Closing File...");
    fclose(file);
}

//Fills up tables of Reserved Words and Signals. Must do it in execution.
void initializeAnalexVariables(){
    printf("\n  Loading Analex variables...\n");
    programLines = 1;
    tblLiteralsCurrentIndex = 0;
    tblCharactersCurrentIndex = 0;
    tblIdentifiersCurrentIndex = 0;
    int i = 0;
    char reservedWordsTemp[tblReservedWordsIndexSize][tblReservedWordsLength] = {"semretorno", "caracter", "inteiro", "real", "booleano", "semparam",
                                                                                "se", "senao", "enquanto", "para", "retorne", "prototipo","fimarquivo"};
    for (i=0;i<=tblReservedWordsIndexSize;i++)
        strcpy(tblReservedWords[i], reservedWordsTemp[i]);

    i = 0;
    char signalsTemp[tblSignalsIndexSize][tblSignalsLength] = {"+", "-", "*", "/", "=", "(", ")", "[", "]", "{", "}", "!",
                                                            "&&", "||", "==", "!=", ">", ">=", "<", "<=", ";", ","};
    for (i=0;i<=tblSignalsIndexSize;i++)
        strcpy(tblSignals[i], signalsTemp[i]);
    printf("  Finished\n\n");
}

int isLetter(char ch) {
    if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z'))) return 1;
    else return 0;
}

int isNumber(char ch) {
    if ((ch >= '0') && (ch <= '9')) return 1;
    else return 0;
}

int isLetterNumberOrUnderline(char ch) {
    if (((ch >= 'a') && (ch <= 'z')) || ((ch >= 'A') && (ch <= 'Z')) || ((ch >= '0')
            && (ch <= '9')) || (ch == '_')) return 1;
    else return 0;
}

void displayMessage(int msgCod) {
    printf("\n - - - - - - - \n\n");
    switch (msgCod) {
        case FILE_END:
            printf("  File has Ended! \n  Total Number of Lines: %d\n", programLines);
            break;
        case FILE_END_ERROR:
            printf("  Error! Unexpected End of File at line %d\n", programLines);
            break;
        case REAL_NOT_FINISHED:
            printf("  Error! Real Constant not finished at line %d\n", programLines);
            break;
        case INVALID_CHARCON:
            printf("  Error! Invalid Character at line %d\n", programLines);
            break;
    }
    printf("\n - - - - - - - \n");
    printf("--   Warning! Machine Code Interrupted!   --\n\n");
    codeGen("<CODE INTERRUPTED>");
    closeCodeFile();
    closeFile();
    system("PAUSE");
    exit(1);
}

void invalidCharacter(char c) {
    printf("Error! Invalid Character Found: %c at line %d\n", c, programLines);
    printf("--   Warning! Machine Code Interrupted!   --\n\n");
    codeGen("<CODE INTERRUPTED>");
    closeCodeFile();
    closeFile();
    system("PAUSE");
    exit(1);
}

//Automaton
token getToken(){
    token tkn;
    char stringTemp[200];
    int indTemp = 0;
    int i = 0;
    automatonState = 0;
    while(1){
        c = getc(file);
        switch(automatonState){
            case 0:
                if ((c == ' ') || (c == '\t')) {
                    ;
                } else if (c == '\n') {
                    programLines++;
                } else if ((c == '(') || (c == ')') || (c == '[') || (c == ']')
                        || (c == '{') || (c == '}') || (c == ';') || (c == ',')
                        || (c == '*') || (c == '+') || (c == '-')) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    stringTemp[indTemp] = '\0';
                    automatonState = 1;
                } else if (c == '/'){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 2;
                } else if (c == '!'){
                    automatonState = 6;
                } else if (c == '='){
                    automatonState = 9;
                } else if (c == '&'){
                    automatonState = 12;
                } else if (c == '|'){
                    automatonState = 14;
                } else if (c == '<'){
                    automatonState = 16;
                } else if (c == '>'){
                    automatonState = 19;
                } else if (isLetter(c)){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 22;
                } else if (isNumber(c)){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 24;
                } else if (c == '\''){
                    automatonState = 29;
                } else if (c == '\"'){
                    automatonState = 37;
                } else if (c == EOF){
                    tkn.ctgr = ENDFILE;
                    tkn.Cod = fileend;
                    return tkn;
                }
                break;
            case 1:
                ungetc(c, file);
                for (i = 0; i < tblSignalsIndexSize; i++) {
                    if (strncmp(stringTemp, tblSignals[i], tblSignalsLength) == 0) {
                        tkn.ctgr = SN;
                        tkn.Cod = i;
                        return tkn;
                    }
                }
                break;
            case 2:
                if (c == '*') {
                    indTemp = 0;
                    automatonState = 3;
                } else {
                    ungetc(c, file);
                    stringTemp[indTemp] = '\0';
                    automatonState = 4;
                }
                break;
            case 3:
                if (c == '*') {
                    automatonState = 5;
                } else if (c == EOF) {
                    displayMessage(FILE_END_ERROR);
                } else if (c == '\n') {
                    programLines++;
                } else {
                    ;
                }
                break;
            case 4:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = divisao;
                return tkn;
                break;
            case 5:
                if (c == '*') {
                    ;
                } else if (c == '/') {
                    automatonState = 0;
                } else if (c == EOF) {
                    displayMessage(FILE_END_ERROR);
                } else {
                    if (c == '\n') {
                        programLines++;
                    }
                    automatonState = 3;
                }
                break;
            case 6:
                if (c == '=') {
                    automatonState = 7;
                } else {
                    ungetc(c, file);
                    automatonState = 8;
                }
                break;
            case 7:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = diferente;
                return tkn;
                break;
            case 8:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = not;
                return tkn;
                break;
            case 9:
                if (c == '=') {
                    automatonState = 10;
                } else {
                    ungetc(c, file);
                    automatonState = 11;
                }
                break;
            case 10:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = igual;
                return tkn;
                break;
            case 11:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = atribuicao;
                return tkn;
                break;
            case 12:
                if (c == '&') {
                    automatonState = 13;
                } else {
                    invalidCharacter('&');
                }
                break;
            case 13:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = and;
                return tkn;
                break;
            case 14:
                if (c == '|') {
                    automatonState = 15;
                } else {
                    invalidCharacter('|');
                }
                break;
            case 15:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = or;
                return tkn;
                break;
            case 16:
                if (c == '=') {
                    automatonState = 17;
                } else {
                    ungetc(c, file);
                    automatonState = 18;
                }
                break;
            case 17:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = menorigual;
                return tkn;
                break;
            case 18:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = menor;
                return tkn;
                break;
            case 19:
                if (c == '=') {
                    automatonState = 20;
                } else {
                    ungetc(c, file);
                    automatonState = 21;
                }
                break;
            case 20:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = maiorigual;
                return tkn;
                break;
            case 21:
                ungetc(c, file);
                tkn.ctgr = SN;
                tkn.Cod = maior;
                return tkn;
                break;
            case 22:
                if ((isLetterNumberOrUnderline(c))) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                } else {
                    ungetc(c, file);
                    stringTemp[indTemp] = '\0';
                    automatonState = 23;
                }
                break;
            case 23:
                ungetc(c, file);
                for (i = 0; i < tblReservedWordsIndexSize; i++) {
                    if (strncmp(stringTemp, tblReservedWords[i], tblReservedWordsLength) == 0) {
                        tkn.ctgr = PR;
                        tkn.Cod = i;
                        return tkn;
                    }
                }
                tkn.ctgr = ID;
                strcpy(tkn.lexema, stringTemp);
                return tkn;
                break;
            case 24:
                if (isNumber(c)) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                } else if (c == '.') {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 26;
                } else {
                    ungetc(c, file);
                    stringTemp[indTemp] = '\0';
                    automatonState = 25;
                }
                break;
            case 25:
                ungetc(c, file);
                tkn.ctgr = CT_I;
                tkn.valueInt = atoi(stringTemp);
                return tkn;
                break;
            case 26:
                if (isNumber(c)) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 27;
                } else {
                    displayMessage(REAL_NOT_FINISHED);
                }
                break;
            case 27:
                if (isNumber(c)) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                } else {
                    ungetc(c, file);
                    stringTemp[indTemp] = '\0';
                    automatonState = 28;
                }
                break;
            case 28:
                ungetc(c, file);
                tkn.ctgr = CT_R;
                tkn.valueReal = atof(stringTemp);
                return tkn;
                break;
            case 29:
                if (c == '\\') {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 32;
                } else if ((isprint(c) != 0) && (c != '\'')){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 30;
                } else {
                    displayMessage(INVALID_CHARCON);
                }

                 //OUTDATED CODE - HERE FOR REFERENCE
                 /*
                if (c == '\\') {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 31;
                } else if ((isprint(c) != 0) && (c != '\'')) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 30;
                } else {
                    displayMessage(INVALID_CHARCON);
                }*/
                break;
            case 30:
                if(c == '\''){
                    stringTemp[indTemp] = '\0';
                    automatonState = 31;
                } else {
                    displayMessage(INVALID_CHARCON);
                }

                 //OUTDATED CODE - HERE FOR REFERENCE
                 /*
                if (c == '\'') {
                    stringTemp[indTemp] = '\0';
                    automatonState = 32;
                } else {
                    displayMessage(INVALID_CHARCON);
                }*/
                break;
            case 31:
                ungetc(c, file);
                tkn.ctgr = CT_C;
                tkn.index = tblCharactersCurrentIndex;
                strcpy(tblCharacters[tblCharactersCurrentIndex], stringTemp);
                tblCharactersCurrentIndex++;
                return tkn;

                 //OUTDATED CODE - HERE FOR REFERENCE
                 /*
                if ((isprint(c) != 0) && (c != '\'')) {
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 30;
                } else {
                    displayMessage(INVALID_CHARCON);
                }*/
                break;
            case 32:
                if (c == '0'){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 33;
                } else if (c == 'n'){
                    stringTemp[indTemp] = c;
                    indTemp++;
                    automatonState = 35;
                } else {
                    displayMessage(INVALID_CHARCON);
                }

                //OUTDATED CODE - HERE FOR REFERENCE
                /*
                ungetc(c, file);
                tkn.ctgr = CT_C;
                tkn.index = tblCharactersCurrentIndex;
                strcpy(tblLiterals[tblCharactersCurrentIndex], stringTemp);
                tblCharactersCurrentIndex++;
                return tkn;
                */
                break;
            case 33:
                if(c == '\''){
                    stringTemp[indTemp] = '\0';
                    automatonState = 34;
                } else {
                    displayMessage(INVALID_CHARCON);
                }
                break;

            case 34:
                ungetc(c, file);
                tkn.ctgr = CT_C;
                tkn.index = tblCharactersCurrentIndex;
                strcpy(tblCharacters[tblCharactersCurrentIndex], "NUL TERMINATOR");
                tblCharactersCurrentIndex++;
                return tkn;
                break;
            case 35:
                if(c == '\''){
                    stringTemp[indTemp] = '\0';
                    automatonState = 36;
                } else {
                    displayMessage(INVALID_CHARCON);
                }
                break;
            case 36:
                ungetc(c, file);
                tkn.ctgr = CT_C;
                tkn.index = tblCharactersCurrentIndex;
                strcpy(tblCharacters[tblCharactersCurrentIndex], "ENTER");
                tblCharactersCurrentIndex++;
                return tkn;
                break;
            case 37:
                if (c == '\"') {
                    stringTemp[indTemp] = '\0';
                    automatonState = 38;
                } else if (c == '\n'){
                    displayMessage(INVALID_CHARCON);
                } else {
                    stringTemp[indTemp] = c;
                    indTemp++;
                }
                break;
            case 38:
                ungetc(c, file);
                tkn.ctgr = LT;
                tkn.index = tblLiteralsCurrentIndex;
                strcpy(tblLiterals[tblLiteralsCurrentIndex], stringTemp);
                tblLiteralsCurrentIndex++;
                return tkn;
                break;
            default:
                invalidCharacter(c);
                break;
        }
    }
}

void printToken(token tkn){
    switch(tkn.ctgr){
        case ID:
            printf("   <ID, %s>\n", tkn.lexema);
            break;
        case PR:
            printf("   <PR, %s>\n", tblReservedWords[tkn.Cod]);
            break;
        case SN:
            printf("   <SN, %s>\n", tblSignals[tkn.Cod]);
            break;
        case CT_I:
            printf("   <CT_I, %d>\n", tkn.valueInt);
            break;
        case CT_R:
            printf("   <CT_R, %f>\n", tkn.valueReal);
            break;
        case CT_C:
            printf("   <CT_C, %s>\n", tblCharacters[tkn.index]);
            break;
        case LT:
            printf("   <LT, %s>\n", tblLiterals[tkn.index]);
            break;
        case ENDFILE:
            printf("   <ENDFILE, %s>\n", tblReservedWords[tkn.Cod]);
            break;
    }
}
