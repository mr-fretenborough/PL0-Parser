#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lex.h"

symbol* parse(lexeme *list)
{
	symbol *table = malloc(500 * sizeof(symbol));
	printf("symbols are parsed\n");
	return table;
}