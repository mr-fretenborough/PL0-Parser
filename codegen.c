#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

instruction* generate_code(symbol *table, lexeme *list)
{
	instruction *code = malloc(500 * sizeof(instruction));
	printf("code has been generated\n");
	return code;
}