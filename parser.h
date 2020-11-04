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

symbol* parse(lexeme *list);

#endif