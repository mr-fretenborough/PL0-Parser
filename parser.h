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

symbol* parse(lexeme *list, int size, int *symbolTableSize);
void program(lexeme *list, symbol *table);
void block(lexeme *list, symbol *table);
void constDeclaration(lexeme *list, symbol *table);
void varDeclaration(lexeme *list, symbol *table);
void statement(lexeme *list, symbol *table);
void condition(lexeme *list, symbol *table);
void expression(lexeme *list, symbol *table);
void term(lexeme *list, symbol *table);
void factor(lexeme *list, symbol *table);

#endif
