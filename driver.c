#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"		//	-------------------------------
#include "codegen.h"	//	havent implemented these yet!!!
#include "vm.h"				//	-------------------------------

/* TODO ::
 * add errors to lexeme analyzer (should just print the error and return null)
 * create the parser
 * create the code generator
 * move the vm over into its own file
*/

int main(int argc, char **argv)
{
	int lFlag = 0, aFlag = 0, vFlag = 0;
	int i;
	int lexemeListSize = 0;

	if (argc < 2)
		printf("error : please include the file name\n");

	FILE *ifp = fopen(argv[1], "r");
	if (ifp == NULL)
	{
		printf("ERROR: Failed to open file\n");
		return 0;
	}

	// sets the flags for what to display
	for (i = 2; i < argc; i++)
	{
		switch (argv[i][1])
		{
			case 'l':
				lFlag = 1;
				break;
			case 'a':
				aFlag = 1;
				break;
			case 'v':
				vFlag = 1;
				break;
		}
	}

	lexeme *list = lex_analyze(ifp, &lexemeListSize);
	if (lFlag)
		printLexemeList(list, lexemeListSize);

	
/*
	symbol *table = parse(list);
	instruction *code = generate_code(table, list);
	virtual_machine(code);
*/

	return 0;
}
