// Kier Fretenborough and Bryce Hitchcock
// COP 3402 Computer Architecture
// Homework #3: PL-0 Parser/CodeGen
// Prof. Mantagne
// 2020 November 8

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
	int codeLength = 0;

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

	// print the source code
	printFile(ifp);

	// creates the lexeme list from an input file
	lexeme *list = lexAnalyze(ifp, &lexemeListSize);

	// if the list returned null that means we encountered an error
	if (list == NULL)
	{
		printf("\nError detected in scanner, exiting...\n");
		return 0;
	}
	// check to see if user wants to print lexeme list
	if (lFlag)
	{
		printLexemeList(list, lexemeListSize);
		// printSymbols(list, lexemeListSize);
		printLexemeTable(list, lexemeListSize);
		printf("\n");
	}

	// generate the symbol table
	symbol *table = parse(list, lexemeListSize, &symbolTableSize);

	// generate the assembly code
	instruction *code = generateCode(table, list, symbolTableSize, lexemeListSize, &codeLength);

	// check to see if the user wants to print the assembly code
	if (aFlag)
	{
		printf("Assembly Code ::\n");
		printAssembly(code, codeLength);
	}

	// check to see if the user wants to print the assembly execution
	if (vFlag)
		virtual_machine(code, codeLength);

	return 0;
}
