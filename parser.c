#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lex.h"

#define MAX_SYMBOL_TABLE_SIZE 1000

// init error flag, lex size, and current token
int errorFlag = 0;
int size, cToken, iSymbol;

// basic functions
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

// helper functions
int checkIdent(char* search, symbol* table);
int isVar(char* search, symbol* table);

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
		printf("\nNo errors, program is syntatically correct\n\n");
	*symbolTableSize = iSymbol;
	return table;
}
//------------------------------------------------------------------------------
void program(lexeme *list, symbol *table) {
	block(list, table);
	if (cToken > size || list[cToken].tokenType != 19) {
		errorFlag = 1;
		printf("ERROR 9: Period expected\n");
		exit(0);
	}
}
//------------------------------------------------------------------------------
void block(lexeme *list, symbol *table) {
	// iterate through the list, looking for const, var, and stmts
	constDeclaration(list, table);
	varDeclaration(list, table);
	statement(list, table);
}
//------------------------------------------------------------------------------
// handles constant declaration error detection
void constDeclaration(lexeme *list, symbol *table) {
	char string[12] = {""};

	// if we find a const statement we must extract the information
	if (cToken < size && list[cToken].tokenType == 28) {
		do {
			// grab the next token and make sure its an identifier
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 2) {
				printf("ERROR NUMBER 4: const, var, procedure must be followed by identifier.\n");
				exit(0);
			}

			// keep a placeholder for the name of the identifier
			strcpy(string, list[cToken].lexeme);

			// make sure the identifier is not already in the symbol table
			if (checkIdent(list[cToken].lexeme, table)) {
				printf("ERROR NUMBER 26: Identifier \"%s\" has already been declared.\n", list[cToken].lexeme);
				exit(0);
			}

			// grab the next toke and make sure its an "="
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 9) {
				printf("ERROR NUMBER 3: Identifier \"%s\" must be followed by \"=\".\n", string);
				exit(0);
			}

			// grab the next token and make sure its a number
			cToken++;
			if (cToken >= size || list[cToken].tokenType != 3) {
				printf("ERROR NUMBER 2: \"=\" must be followed by a number.\n");
				exit(0);
			}

			// fill out the symbol table with the appropriate values and move both the
			// symbol table and the token index over
			table[iSymbol].kind = 1;
			strcpy(table[iSymbol].name, string);
			table[iSymbol].val = atoi(list[cToken].lexeme);
			table[iSymbol].level = 0;
			table[iSymbol].addr = 0;
			table[iSymbol].mark = 0;
			iSymbol++;
			cToken++;

		} while (list[cToken].tokenType == 17); // checks for another const

		// make sure that the next token is a ";"
		if (cToken >= size || list[cToken].tokenType != 18) {
			printf("ERROR NUMBER 5: Semicolon or comma missing.\n");
			exit(0);
		} else {
			cToken++;
		}

	} else {
		return;
	}
}
//------------------------------------------------------------------------------
// handles variable declaration error detection
void varDeclaration(lexeme *list, symbol *table) {
	// if token is a var, process following tokens
	if (cToken < size && list[cToken].tokenType == 29) {
		// init qVar at 2 for some reason i forgot, lmao
		int qVar = 2;
		do {
			qVar++;
			// if following token is not an ident, raise error
			if (cToken >= size || list[++cToken].tokenType != 2) {
				printf("ERROR NUMBER 4: const, var, procedure must be followed by identifier.\n");
				exit(0);
			// if following token is not a duplicate
			} else if (!checkIdent(list[cToken].lexeme, table)) {
				// populate a new table entry
				table[iSymbol].kind = 2;
				strcpy(table[iSymbol].name, list[cToken].lexeme);
				table[iSymbol].val = 0;
				table[iSymbol].level = 0;
				table[iSymbol].addr = qVar;
				table[iSymbol].mark = 0;
				// increment table index
				iSymbol++;
				cToken++;
			} else if (checkIdent(list[cToken].lexeme, table)) {
				// there is a duplicate value, print error and exit
				printf("ERROR NUMBER 26: Identifier \"%s\" has already been declared.\n", list[cToken].lexeme);
				exit(0);
			}
			// continue if next token is a ","
		} while (list[cToken].tokenType == 17);
		// if the next token is not an ";", raise error
		if (cToken >= size || list[cToken].tokenType != 18) {
			printf("ERROR NUMBER 5: Semicolon or comma missing.\n");
			exit(0);
		// get next token and return
		} else {
			cToken++;
		}
	} else {
		return;
	}
}
//------------------------------------------------------------------------------
// checks for existence of symbol, return boolean (0-no dupe, 1-dupe)
int checkIdent(char* search, symbol* table) {
	// check to see if ident is already in the table
	for (int i = 0; i < iSymbol; i++) {
		if (strcmp(search, table[i].name) == 0) {
			// there is a duplicate value, return true (1)
			return 1;
		}
	}
	// no duplicates, return false (0)
	return 0;
}
//------------------------------------------------------------------------------
void statement(lexeme *list, symbol *table) {
	// if token is an ident, process
	if (list[cToken].tokenType == 2) {
		// if token !in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 11: Undeclared identifier.\n");
			exit(0);
		}
		if (!isVar(list[cToken].lexeme, table)) { // if ident != var, raise error
			printf("ERROR NUMBER 12: Assignment to constant or procedure is not allowed.\n");
			exit(0);
		}
		cToken++;
		// if token not ":=", raise error
		if (list[cToken].tokenType != 20) {
			printf("ERROR NUMBER 13: Assignment operator expected.\n");
			exit(0);
		}
		cToken++;
		expression(list, table);
		return;
	}
	// if token is "begin", process
	if (list[cToken].tokenType == 21) {
		cToken++;
		statement(list, table);
		// while token is ";", process
		while (list[cToken].tokenType == 18) {
			cToken++;
			statement(list, table);
		}
		// if token not "end", raise error
		if (list[cToken].tokenType != 22) {
			printf("ERROR NUMBER 27: End expected\n");
			exit(0);
		}
		cToken++;
		return;
	}
	// if token "if", process
	if (list[cToken].tokenType == 23) {
		cToken++;
		condition(list, table);
		// if token not "then", raise error
		if (list[cToken].tokenType != 24) {
			printf("ERROR NUMBER 16: then expected.\n");
			exit(0);
		}
		cToken++;
		statement(list, table);
		return;
	}
	// if token "while", process
	if (list[cToken].tokenType == 25) {
		cToken++;
		condition(list, table);
		// if token not "do", raise error
		if (list[cToken].tokenType != 26) {
			printf("ERROR NUMBER 18: do expected.\n");
			exit(0);
		}
		cToken++;
		statement(list, table);
		return;
	}
	// if token "read", process
	if (list[cToken].tokenType == 32) {
		cToken++;
		// if token not ID, raise error
		if (list[cToken].tokenType != 2) {
			printf("ERROR NUMBER 28: Identifier expected after read\n");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 11: Undeclared identifier.\n");
			exit(0);
		}
		// if ID not a var
		if (!isVar(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 29: Can only read to a var.\n");
			exit(0);
		}
		cToken++;
		return;
	}
	// if token is "write", process
	if (list[cToken].tokenType == 31) {
		cToken++;
		// if token not ID, raise error
		if (list[cToken].tokenType != 2) {
			printf("ERROR NUMBER 30: Identifier expected after write\n");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 11: Undeclared identifier.\n");
			exit(0);
		}
		cToken++;
		return;
	}
	return;
}
//------------------------------------------------------------------------------
// handles conditional error detection
void condition(lexeme *list, symbol *table) {
	if (list[cToken].tokenType == 8) { // if token == odd
		cToken++;
		expression(list, table);
	} else {
		expression(list, table);
		// if token is not a comparison, raise error
		if (!(list[cToken].tokenType == 9		// "="
		 || list[cToken].tokenType == 10	// "<>"
		 || list[cToken].tokenType == 11	// "<"
		 || list[cToken].tokenType == 12	// "<="
		 || list[cToken].tokenType == 13	// ">"
		 || list[cToken].tokenType == 14)) {	// ">="
			 printf("ERROR NUMBER 20: Relational operator expected.\n");
			 exit(0);
		 } // figure out these codes
		 cToken++;
		 expression(list, table);
	}
}
//------------------------------------------------------------------------------
// handles expressional (arithmetic) error detection
void expression(lexeme *list, symbol *table) {
	// if token is "+" or "-", get next and process
	if (list[cToken].tokenType == 4 || list[cToken].tokenType == 5) {
		cToken++;
	}
	term(list, table);
	// while token is "+" or "-", get next and process
	while (list[cToken].tokenType == 4 || list[cToken].tokenType == 5) {
		cToken++;
		term(list, table);
	}
}

// processes arithmetic and terms
void term(lexeme *list, symbol *table) {
	factor(list, table);
	// while token is "*" or "/", process
	while (list[cToken].tokenType == 6 || list[cToken].tokenType == 7) {
		cToken++;
		factor(list, table);
	}
}
//------------------------------------------------------------------------------
// handles arithmetic error detection
void factor(lexeme *list, symbol *table) {
	// if token is an identifier
	if (list[cToken].tokenType == 2) {
		// if the ID is not in the table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("ERROR NUMBER 11: Undeclared identifier.\n");
			exit(0);
		}
		cToken++;
	// if token is a number, get next
	} else if (list[cToken].tokenType == 3) {
		cToken++;
	// if token is a "(", process
	} else if (list[cToken].tokenType == 15) {
		cToken++;
		expression(list, table);
		// if token is not ")", raise error
		if (list[cToken].tokenType != 16) {
			printf("ERROR NUMBER 22: Right parenthesis missing.\n");
			exit(0);
		}
		cToken++;
	} else {
		printf("ERROR NUMBER 23: The preceding factor cannot begin with this symbol.\n");
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
