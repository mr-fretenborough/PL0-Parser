#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct instruction
{
	int opcode;
	char *op;
	int l;
	int m;
	
} instruction;

instruction* generate_code(symbol *table, lexeme *list);

#endif