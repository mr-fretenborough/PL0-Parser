#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "lex.h"

#define MAX_SYMBOL_TABLE_SIZE 1000

// init error flag, lex size, and current token
int errorFlag = 0;
int size, cToken, symbolIndex;

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
	// init symbol table and current token
	symbol *table = malloc(MAX_SYMBOL_TABLE_SIZE * sizeof(symbol));
	cToken = 0;
	size = inputSize;
	symbolIndex = 0;

	program(list, table);

	// if there are no erros, print the success message
	if (!errorFlag)
		printf("symbols are parsed\n");
	*symbolTableSize = symbolIndex;
	return table;
}

void program(lexeme *list, symbol *table) {
	block(list, table);
	if (cToken > size || strcmp(list[cToken].lexeme, ".") != 0) {
		errorFlag = 1;
		printf("Error number 9, period expected\n");
		// exit(0);
	}
}

void block(lexeme *list, symbol *table) {
	// iterate through the list, looking for const, var, and stmts
	// reset the cToken after each iteration, return to beginning
	constDeclaration(list, table);
	varDeclaration(list, table);
	// statement(list, table);
}

void constDeclaration(lexeme *list, symbol *table) {
	char string[12] = {""};

	if (list[cToken].tokenType == 28) {
		do {
			cToken++;
			if (cToken > size || list[cToken].tokenType != 2) {
				printf("NEED AN IDENTIFIER\n");
				exit(0);
			}

			strcpy(string, list[cToken].lexeme);

			checkIdent(list[cToken].lexeme, table);

			cToken++;

			if (cToken > size || list[cToken].tokenType != 9) {
				printf("NEED AN =\n");
				exit(0);
			}

			cToken++;

			if (cToken > size || list[cToken].tokenType != 3) {
				printf("NEED A NUMBER\n");
				exit(0);
			}

			table[symbolIndex].kind = 1;
			strcpy(table[symbolIndex].name, string);
			table[symbolIndex].val = atoi(list[cToken].lexeme);
			table[symbolIndex].level = 0;
			table[symbolIndex].addr = 0;
			table[symbolIndex].mark = 0;
			symbolIndex++;
			cToken++;

		} while(strcmp(list[cToken].lexeme, ",") == 0);
			if (cToken > size || list[cToken].tokenType != 18) {
				printf("NEED A ;\n");
				exit(0);
			}
	} else {
		return;
	}
}

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
				table[symbolIndex].kind = 1;
				strcpy(table[symbolIndex].name, list[cToken].lexeme);
				table[symbolIndex].val = 0;
				table[symbolIndex].level = 0;
				table[symbolIndex].addr = qVar;
				table[symbolIndex].mark = 0;
				// increment table index
				symbolIndex++;
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

int checkIdent(char* search, symbol* table) {
	// check to see if ident is already in the table
	for (int i = 0; i < symbolIndex; i++) {
		if (strcmp(search, table[symbolIndex].name) == 0) {
			// there is a duplicate value, return true (1)
			return 1;
		}
	}
	// no duplicates, return false (0)
	return 0;
}

void statement(lexeme *list, symbol *table) {
	if (list[cToken].tokenType == 2) {


	}

}

void condition(lexeme *list, symbol *table) {
	if (1) { // figure out wtf "odd" means
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

void term(lexeme *list, symbol *table) {
	factor(list, table);
	// while token is "*" or "/", process
	while (list[cToken].tokenType == 6 || list[cToken].tokenType == 7) {
		cToken++;
		factor(list, table);
	}
}

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
