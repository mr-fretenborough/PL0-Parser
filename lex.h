#ifndef __LEX_H
#define __LEX_H
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

typedef struct lexeme {
    char lexeme[100];
    int tokenType;
} lexeme;

lexeme* lex_analyze(FILE *ifp, int *listCount);
void printFile(FILE *ifp);
int  isIdentifierValid(char *string, lexeme *list, int *counter);
int  isKeyWord(char *string, lexeme *list, int *counter);
void addCharacter(char *string, char c);
int  isNumberValid(char *string, lexeme *list, int *count);
int  containsLetter(char *string);
void printLexemeTable(lexeme *list, int size);
void printLexemeList(lexeme *list, int size);
int isValidSpecial(char c);

#endif
