#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lex.h"

#define MAX_SYMBOL_TABLE_SIZE 1000

// init error flag, lex size, and current token
int errorFlag = 0;
int size, cToken, iSymbol;

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



// helper functions
int checkIdent  (char* search, symbol* table, int lexLevel);
int isVar       (char* search, symbol* table);

//------------------------------------------------------------------------------
symbol* parse(lexeme *list, int inputSize, int *symbolTableSize)
{
	// init symbol table, current token, list length, and symIndex
	symbol *table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));
	cToken = 0;
	size = inputSize;
	iSymbol = 0;

	program(list, table);

	// if there are no errors, print the success message
	if (!errorFlag)
		printf("\nNo errors, program is syntactically correct\n\n");
	*symbolTableSize = iSymbol;
	return table;
}
//------------------------------------------------------------------------------
void program(lexeme *list, symbol *table) {
	// add main to the symbol table
	table[iSymbol].kind = 3;
	strcpy(table[iSymbol].name, "main");
	table[iSymbol].val = 0;
	table[iSymbol].level = 0;
	table[iSymbol].addr = 0;
	table[iSymbol].mark = 0;
	iSymbol++;

	block(list, table, 0);
	if (cToken > size || list[cToken].tokenType != 19) {
		errorFlag = 1;
		printf("ERROR NUMBER 5: Period expected\n");
		exit(0);
	}
}
//------------------------------------------------------------------------------
void block(lexeme *list, symbol *table, int lexLevel) {
	int numSymbols = 0;
	// iterate through the list, looking for const, var, and stmts
	numSymbols += constDeclaration(list, table, lexLevel);
	numSymbols += varDeclaration(list, table, lexLevel);
	numSymbols += procedureDeclaration(list, table, lexLevel);
	statement(list, table, lexLevel);
	// IMPORTANT MARK THE LAST numSymbols of unmared symbols
}
//------------------------------------------------------------------------------
// handles constant declaration error detection
int constDeclaration(lexeme *list, symbol *table, int lexLevel) {
	char string[12] = {""};

	// if we find a const statement we must extract the information
	if (cToken < size && list[cToken].tokenType == 28) {
		do {
			// grab the next token and make sure its an identifier
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 2) {
				printf("ERROR NUMBER 3: const, var, procedure must be followed by identifier.\n");
				exit(0);
			}

			// keep a placeholder for the name of the identifier
			strcpy(string, list[cToken].lexeme);

			// make sure the identifier is not already in the symbol table
			if (checkIdent(list[cToken].lexeme, table, lexLevel)) {
				printf("ERROR NUMBER 15: Identifier \"%s\" has already been declared.\n", list[cToken].lexeme);
				exit(0);
			}

			// grab the next toke and make sure its an "="
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 9) {
				printf("ERROR NUMBER 2: Identifier \"%s\" must be followed by \"=\".\n", string);
				exit(0);
			}

			// grab the next token and make sure its a number
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 3) {
				printf("ERROR NUMBER 1: \"=\" must be followed by a number.\n");
				exit(0);
			}

			// fill out the symbol table with the appropriate values and move both the
			// symbol table and the token index over
			table[iSymbol].kind = 1;
			strcpy(table[iSymbol].name, string);
			table[iSymbol].val = atoi(list[cToken].lexeme);
			table[iSymbol].level = lexLevel;
			table[iSymbol].addr = 0;
			table[iSymbol].mark = 0;
			iSymbol++;
			cToken++;

		} while (list[cToken].tokenType == 17); // checks for another const

		// make sure that the next token is a ";"
		if (cToken >= size || list[cToken].tokenType != 18) {
			printf("ERROR NUMBER 4: Semicolon or comma missing.\n");
			exit(0);
		} else {
			cToken++;
		}

	}
}
//------------------------------------------------------------------------------
// handles variable declaration error detection
int varDeclaration(lexeme *list, symbol *table, int lexLevel) {
	int qVar = 0;
	// if token is a var, process following tokens
	if (cToken < size && list[cToken].tokenType == 29) {
		// init qVar at 2 for some reason i forgot, lmao
		qVar = 2;
		do {
			qVar++;
			// if following token is not an ident, raise error
			if (cToken >= size || list[++cToken].tokenType != 2) {
				printf("ERROR NUMBER 3: const, var, procedure must be followed by identifier.\n");
				exit(0);
			// if following token is not a duplicate
			} else if (!checkIdent(list[cToken].lexeme, table, lexLevel)) {
				// populate a new table entry
				table[iSymbol].kind = 2;
				strcpy(table[iSymbol].name, list[cToken].lexeme);
				table[iSymbol].val = 0;
				table[iSymbol].level = lexLevel;
				table[iSymbol].addr = qVar;
				table[iSymbol].mark = 0;
				// increment table index
				iSymbol++;
				cToken++;
			} else if (checkIdent(list[cToken].lexeme, table, lexLevel)) {
				// there is a duplicate value, print error and exit
				printf("ERROR NUMBER 15: Identifier \"%s\" has already been declared.\n", list[cToken].lexeme);
				exit(0);
			}
			// continue if next token is a ","
		} while (list[cToken].tokenType == 17);
		// if the next token is not an ";", raise error
		if (cToken >= size || list[cToken].tokenType != 18) {
			printf("ERROR NUMBER 4: Semicolon or comma missing.\n");
			exit(0);
		// get next token and return
		} else {
			cToken++;
		}
	}
	return qVar;
}
//------------------------------------------------------------------------------
int procedureDeclaration(lexeme *list, symbol *table, int lexLevel)
{
	// if the token is a procedure
	if (list[cToken].tokenType == 30)
	{
		do {
			// nab the next token
			cToken++;

			// check if the token is an identifier
			if (cToken >= size || list[cToken].tokenType != 2) {
				printf("ERROR NUMBER 3: const, var, procedure must be followed by identifier.\n");
				exit(0);
			}

			// if the ident is already in the symbol table and unmarked at the same lexLevel then throw error
			if (checkIdent(list[cToken].lexeme, table, lexLevel) {
				printf("ERROR NUMBER 15: Identifier \"%s\" has already been declared.\n", list[cToken].lexeme);
				exit(0);
			}

			// add the symbol to the symbol table
			table[iSymbol].kind = 3;
			strcpy(table[iSymbol].name, list[cToken].lexeme);
			table[iSymbol].val = 0;
			table[iSymbol].level = lexLevel;
			table[iSymbol].addr = 0;
			table[iSymbol].mark = 0;
			iSymbol++;
			cToken++;

			// make sure the next token is a ";"
			if (cToken >= size || list[cToken].tokenType != 18) {
				printf("ERROR NUMBER 4: Semicolon or comma missing.\n");
				exit(0);
			}

			// nab the next token
			cToken++;

			// call block at this lex level
			block(list, table, lexLevel);

			// make sure the next token is a ";"
			if (cToken >= size || list[cToken].tokenType != 18) {
				printf("ERROR NUMBER 4: Semicolon or comma missing.\n");
				exit(0);
			}

			// nab the next token
			cToken++;
		} while (list[cToken].tokenType == 30);
	}
}
//------------------------------------------------------------------------------
// checks for existence of symbol, return boolean (0-no dupe, 1-dupe)
int checkIdent(char* search, symbol* table, int lexLevel) {
	// check to see if ident is already in the table
	for (int i = 0; i < iSymbol; i++) {
		if (strcmp(search, table[i].name) == 0 && table[i].level == lexLevel && table[i].mark == 0) {
			// there is a duplicate value, return true (1)
			return 1;
		}
	}
	// no duplicates, return false (0)
	return 0;
}
//------------------------------------------------------------------------------
void statement(lexeme *list, symbol *table, int lexLevel) {
	// if token is an ident, process
	if (list[cToken].tokenType == 2) {

		// if token !in table, raise error
		if (!findIdent(list, table, lexLevel, list[cToken].lexeme)) {
			printf("ERROR NUMBER 6: Undeclared identifier.\n");
			exit(0);
		}

		// grab the next token
		cToken++;

		// if token not ":=", raise error
		if (list[cToken].tokenType != 20) {
			printf("ERROR NUMBER 8: Assignment operator expected.\n");
			exit(0);
		}

		// grab the next token
		cToken++;

		expression(list, table, lexLevel);
		return;
	}
	// if the token is a call, process
	if (list[cToken].tokenType == 27) {
		// grab the next token
		cToken++;

		// if the procedure is not in the symbol table
		if (!findProc(list, table, lexLevel, list[cToken].lexeme)) {
			printf("ERROR NUMBER 8: Assignment operator expected.\n");
			exit(0);
		}

		// grab the next token
		cToken++;
		return;
	}
	// if token is "begin", process
	if (list[cToken].tokenType == 21) {
		cToken++;
		statement(list, table, lexLevel);
		// while token is ";", process
		while (list[cToken].tokenType == 18) {
			cToken++;
			statement(list, table, lexLevel);
		}
		// if token not "end", raise error
		if (list[cToken].tokenType != 22) {
			printf("ERROR NUMBER 16: End expected\n");
			exit(0);
		}
		cToken++;
		return;
	}
	// if token "if", process
	if (list[cToken].tokenType == 23) {
		cToken++;
		condition(list, table, lexLevel);
		// if token not "then", raise error
		if (list[cToken].tokenType != 24) {
			printf("ERROR NUMBER 9: then expected.\n");
			exit(0);
		}
		cToken++;
		statement(list, table, lexLevel);
		// if the token is an else
		if (list[cToken].tokenType == 33) {
			cToken++;
			statement(list, table, lexLevel);
		}
		return;
	}
	// if token "while", process
	if (list[cToken].tokenType == 25) {
		cToken++;
		condition(list, table, lexLevel);
		// if token not "do", raise error
		if (list[cToken].tokenType != 26) {
			printf("ERROR NUMBER 10: do expected.\n");
			exit(0);
		}
		cToken++;
		statement(list, table, lexLevel);
		return;
	}
	// if token "read", process
	if (list[cToken].tokenType == 32) {
		cToken++;

		// work backwards to find the latest unmarked var with the same name
		if (!findIdent(list, table, lexLevel, list[cToken].lexeme)) {
			printf("ERROR NUMBER 6: Undeclared identifier.\n");
			exit(0);
		}

/*
		// if token not ID, raise error
		if (list[cToken].tokenType != 2) {
			printf("ERROR NUMBER 17: Identifier expected after read\n");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 6: Undeclared identifier.\n");
			exit(0);
		}
		// if ID not a var
		if (!isVar(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 18: Can only read to a var.\n");
			exit(0);
		}
*/
		cToken++;
		return;
	}
	// if token is "write", process
	if (list[cToken].tokenType == 31) {
		cToken++;

		expression(list, table, lexLevel);
/*
		// if token not ID, raise error
		if (list[cToken].tokenType != 2) {
			printf("ERROR NUMBER 19: Identifier expected after write\n");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 6: Undeclared identifier.\n");
			exit(0);
		}

		cToken++;
*/
		return;
	}
	return;
}
//------------------------------------------------------------------------------
// handles conditional error detection
void condition(lexeme *list, symbol *table, int lexLevel) {
	if (list[cToken].tokenType == 8) { // if token == odd
		cToken++;
		expression(list, table, lexLevel);
	} else {
		expression(list, table, lexLevel);
		// if token is not a comparison, raise error
		if (!(list[cToken].tokenType == 9		// "="
		 || list[cToken].tokenType == 10	// "<>"
		 || list[cToken].tokenType == 11	// "<"
		 || list[cToken].tokenType == 12	// "<="
		 || list[cToken].tokenType == 13	// ">"
		 || list[cToken].tokenType == 14)) {	// ">="
			 printf("ERROR NUMBER 11: Relational operator expected.\n");
			 exit(0);
		 } // figure out these codes
		 cToken++;
		 expression(list, table, lexLevel);
	}
}
//------------------------------------------------------------------------------
// handles expressional (arithmetic) error detection
void expression(lexeme *list, symbol *table, int lexLevel) {
	// if token is "+" or "-", get next and process
	if (list[cToken].tokenType == 4 || list[cToken].tokenType == 5) {
		cToken++;
	}
	term(list, table, lexLevel);
	// while token is "+" or "-", get next and process
	while (list[cToken].tokenType == 4 || list[cToken].tokenType == 5) {
		cToken++;
		term(list, table, lexLevel);
	}
}

// processes arithmetic and terms
void term(lexeme *list, symbol *table, int lexLevel) {
	factor(list, table, lexLevel);
	// while token is "*" or "/", process
	while (list[cToken].tokenType == 6 || list[cToken].tokenType == 7) {
		cToken++;
		factor(list, table, lexLevel);
	}
}
//------------------------------------------------------------------------------
// handles arithmetic error detection
void factor(lexeme *list, symbol *table, int lexLevel) {
	// if token is an identifier
	if (list[cToken].tokenType == 2) {
		// if the ID is not in the table, raise error
		if (!findVarOrConst(list, table, lexLevel, list[cToken].lexeme)) {
			printf("ERROR NUMBER 6: Undeclared identifier.\n");
			exit(0);
		}
		cToken++;
	// if token is a number, get next
	} else if (list[cToken].tokenType == 3) {
		cToken++;
	// if token is a "(", process
	} else if (list[cToken].tokenType == 15) {
		cToken++;
		expression(list, table, lexLevel);
		// if token is not ")", raise error
		if (list[cToken].tokenType != 16) {
			printf("ERROR NUMBER 12: Right parenthesis missing.\n");
			exit(0);
		}
		cToken++;
	} else {
		printf("ERROR NUMBER 13: The preceding factor cannot begin with this symbol.\n");
		exit(0);
	}
}
//-----------------------------------------------------------------------------
// takes in a string and a symbol table and returns 1 if the string is a var otherwise false
int isVar(char* search, symbol* table)
{
	int i;

	for (i = 0; i < iSymbol; i++) {
		if (strcmp(table[i].name, search) == 0 && table[i].kind == 2)
			return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int findIdent(lexeme *list, symbol *table, int lexLevel, char *name)
{
	int i = 0;

	// loop through the symbol table backwards and find the latest
	for (i = iSymbol - 1; i >= 0; i++)
	{
		// if current table entry has the same name, is a var, and is unmarked then return true
		if (strcmp(table[i].name, name) == 0 && table[i].kind == 2 && table[i].mark == 0)
			return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int findProc(lexeme *list, symbol *table, int lexLevel, char *name)
{
	int i = 0;

	// loop through the symbol table backwards and find the latest
	for (i = iSymbol - 1; i >= 0; i++)
	{
		// if current table entry has the same name, is a var, and is unmarked then return true
		if (strcmp(table[i].name, name) == 0 && table[i].kind == 3 && table[i].mark == 0)
			return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
int findVarOrConst(lexeme *list, symbol *table, int lexLevel, char *name)
{
	int i = 0;

	// loop through the symbol table backwards and find the latest
	for (i = iSymbol - 1; i >= 0; i++)
	{
		// if current table entry has the same name, is a var, and is unmarked then return true
		if (strcmp(table[i].name, name) == 0 && (table[i].kind == 2 || table[i].kind == 1) && table[i].mark == 0)
			return 1;
	}
	return 0;
}
