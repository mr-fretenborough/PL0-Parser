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

/* ERROR List
 * when the identifier is too long
 * when the number is too long
 * when an identifier starts with a containsLetter
 * when the sysmbol isnt in out language
*/

int main(int argc, char **argv)
{
	int lFlag = 0, aFlag = 0, vFlag = 0;
	int i;
	int lexemeListSize = 0;
	int symbolTableSize = 0;

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

	lexeme *list = lexAnalyze(ifp, &lexemeListSize);
	if (list == NULL)
	{
		printf("\nError detected in scanner, exiting...\n");
		return 0;
	}

	if (lFlag)
	{
		printLexemeList(list, lexemeListSize);
		printSymbols(list, lexemeListSize);
	}





	symbol *table = parse(list, lexemeListSize, &symbolTableSize);

	for (i = 0; i < 2; i++)
 	{
		printf("KIND:: %d\nNAME:: %s\n\n", table[i].kind, table[i].name);
	}

/*
	instruction *code = generate_code(table, list);
	virtual_machine(code);
*/

	return 0;
}
