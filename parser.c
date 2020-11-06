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
		printf("COMPLETED: Symbols have been parsed\n");
	*symbolTableSize = iSymbol;
	return table;
}

void program(lexeme *list, symbol *table) {
	block(list, table);
	if (cToken > size || list[cToken].tokenType == 19) {
		errorFlag = 1;
		printf("ERROR 9: Period expected\n");
		exit(0);
	}
}

void block(lexeme *list, symbol *table) {
	// iterate through the list, looking for const, var, and stmts
	constDeclaration(list, table);
	varDeclaration(list, table);
	statement(list, table);
}

// handles constant declaration error detection
void constDeclaration(lexeme *list, symbol *table) {
	char string[12] = {""};

	if (list[cToken].tokenType == 28) {
		do {
			cToken++;
			if (cToken > size || list[cToken].tokenType != 2) {
				printf("ERROR XX: Need an identifier\n");
				exit(0);
			}

			strcpy(string, list[cToken].lexeme);
///////////////////////////////////////////////////
			checkIdent(list[cToken].lexeme, table); // i updated checkIdent to return a value
///////////////////////////////////////////////////
			cToken++;

			if (cToken > size || list[cToken].tokenType != 9) {
				printf("ERROR XX: Need an \"=\"\n");
				exit(0);
			}

			cToken++;

			if (cToken > size || list[cToken].tokenType != 3) {
				printf("ERROR XX: Need a number\n");
				exit(0);
			}

			table[iSymbol].kind = 1;
			strcpy(table[iSymbol].name, string);
			table[iSymbol].val = atoi(list[cToken].lexeme);
			table[iSymbol].level = 0;
			table[iSymbol].addr = 0;
			table[iSymbol].mark = 0;
			iSymbol++;
			cToken++;

		} while(strcmp(list[cToken].lexeme, ",") == 0);
			if (cToken > size || list[cToken].tokenType != 18) {
				printf("ERROR XX: Need a \";\"\n");
				exit(0);
			}
	} else {
		return;
	}
}

// handles variable declaration error detection
void varDeclaration(lexeme *list, symbol *table) {
	// if token is a var, process following tokens
	if (list[cToken].tokenType == 29) {
		// init qVar at 2 for some reason i forgot, lmao
		int qVar = 2;
		do {
			qVar++;
			// if following token is not an ident, raise error
			if (list[cToken++].tokenType != 2) {
				printf("some mf errors bitch");
				exit(0);
			// if following token is not a duplicate
			} else if (!checkIdent(list[cToken].lexeme, table)) {
				// populate a new table entry
				table[iSymbol].kind = 1;
				strcpy(table[iSymbol].name, list[cToken].lexeme);
				table[iSymbol].val = 0;
				table[iSymbol].level = 0;
				table[iSymbol].addr = qVar;
				table[iSymbol].mark = 0;
				// increment table index
				iSymbol++;
			} else if (checkIdent(list[cToken].lexeme, table)) {
				// there is a duplicate value, print error and exit
				printf("damn bud issa error");
				exit(0);
			}
			// continue if next token is a ","
		} while (list[cToken++].tokenType == 17);
	// if the next token is not an ";", raise error
	} else if (list[cToken].tokenType != 18) {
		printf("bruh fuck these errors");
		exit(0);
	// get next token and return
	} else {
		cToken++;
	}
}

// checks for existence of symbol, return boolean (0-no dupe, 1-dupe)
int checkIdent(char* search, symbol* table) {
	// check to see if ident is already in the table
	for (int i = 0; i < iSymbol; i++) {
		if (strcmp(search, table[iSymbol].name) == 0) {
			// there is a duplicate value, return true (1)
			return 1;
		}
	}
	// no duplicates, return false (0)
	return 0;
}

/*/////////////////////////////////////////
	I have just written the rough draft
	for the statement function, I haven't
	done any debugging and the logic
	for lines 174 and 245 have to be
	specified.
/////////////////////////////////////////*/
void statement(lexeme *list, symbol *table) {
	// if token is an ident, process
	if (list[cToken].tokenType == 2) {
		// if token !in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("got errors on errors");
			exit(0);
		}
		if (1) { // if ident != var, raise error
			printf("rollin in errors");
			exit(0);
		}
		cToken++;
		// if token not ":=", raise error
		if (list[cToken].tokenType == 20) {
			printf("fuck");
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
			printf("shit");
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
			printf("bitch");
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
			printf("young sheck wes,");
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
			printf("and we getting");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("really rich");
			exit(0);
		}
		// if ID not a var
		if (1) {
			printf("see how i caught it");
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
			printf("cause i be really");
			exit(0);
		}
		// if ID not in table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("with the shits");
			exit(0);
		}
		cToken++;
		return;
	}
	return;
}

// handles conditional error detection
void condition(lexeme *list, symbol *table) {
	if (list[cToken].tokenType != 8) { // if token == odd ????????
		cToken++;
		expression(list, table);
	} else {
		expression(list, table);
		// if token is not a comparison, raise error
		if (list[cToken].tokenType != 9		// "="
		 || list[cToken].tokenType != 10	// "<>"
		 || list[cToken].tokenType != 11	// "<"
		 || list[cToken].tokenType != 12	// "<="
		 || list[cToken].tokenType != 13	// ">"
		 || list[cToken].tokenType != 14) {	// ">="
			 printf("damn dis do be a error");
			 exit(0);
		 } // figure out these codes
		 cToken++;
		 expression(list, table);
	}
}

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

// handles arithmetic error detection
void factor(lexeme *list, symbol *table) {
	// if token is an identifier
	if (list[cToken].tokenType == 2) {
		// if the ID is not in the table, raise error
		if (!checkIdent(list[cToken].lexeme, table)) {
			printf("dude got sum errors");
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
			printf("fuck off ya error");
			exit(0);
		}
		cToken++;
	} else {
		printf("god damn w the errors");
		exit(0);
	}
}
