#ifndef __PARSER_H
#define __PARSER_H

#include "lex.h"

typedef struct symbol
{
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
	int mark;
} symbol;

// basic functions
symbol* parse           	(lexeme *list, int size, int *symbolTableSize);
void program            	(lexeme *list, symbol *table);
void block              	(lexeme *list, symbol *table, int lexLevel);
int constDeclaration   		(lexeme *list, symbol *table, int lexLevel);
int varDeclaration     		(lexeme *list, symbol *table, int lexLevel);
void statement          	(lexeme *list, symbol *table, int lexLevel);
void condition          	(lexeme *list, symbol *table, int lexLevel);
void expression         	(lexeme *list, symbol *table, int lexLevel);
void term               	(lexeme *list, symbol *table, int lexLevel);
void factor             	(lexeme *list, symbol *table, int lexLevel);
int procedureDeclaration	(lexeme *list, symbol *table, int lexLevel);
int findIdent							(lexeme *list, symbol *table, int lexLevel, char *name);
int findProc							(lexeme *list, symbol *table, int lexLevel, char *name);
int findVarOrConst				(lexeme *list, symbol *table, int lexLevel, char *name);
void markSymbols					(lexeme *list, symbol *table, int numSymbols);


// helper functions
int checkIdent  (char* search, symbol* table, int lexLevel);
int isVar       (char* search, symbol* table);

#endif
