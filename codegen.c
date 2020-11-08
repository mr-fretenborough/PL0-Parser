#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define CODE_SIZE 500

// tSize is the size of the symbol table, lsize is the size of the lexeme list,
// and cx is the current index of the code array
int tSize, lSize, cx, cToken;

// basic functions
instruction* generateCode(symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize);
void genProgram				(symbol *table, lexeme *list, instruction *code);
void genBlock					(symbol *table, lexeme *list, instruction *code);
void genStatement			(symbol *table, lexeme *list, instruction *code);
void genCondition			(symbol *table, lexeme *list, instruction *code);
void genExpression		(symbol *table, lexeme *list, instruction *code, int reg);
void genTerm					(symbol *table, lexeme *list, instruction *code, int reg);
void genFactor				(symbol *table, lexeme *list, instruction *code, int reg);
void emit							(char *op, int opcode, int r, int l, int m, instruction *code);
int getTableIndex			(symbol *table, char *target);


//------------------------------------------------------------------------------
instruction* generateCode(symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize)
{
	instruction *code = malloc(CODE_SIZE * sizeof(instruction));

	tSize = tableSize;
	lSize = listSize;
	cToken = 0;
	cx = 0;

	genProgram(table, list, code);

	*codeSize = cx;
	printf("\n\ncode has been generated");
	return code;
}
//------------------------------------------------------------------------------
void genProgram(symbol *table, lexeme *list, instruction *code) {
	emit("JMP", 7, 0, 0, 1, code);
	genBlock(table, list, code);
	emit("SYS", 9, 0, 0, 3, code);
}
//------------------------------------------------------------------------------
void genBlock(symbol *table, lexeme *list, instruction *code) {
	int numVars = 0;

	// if the current token is a const
	if (list[cToken].tokenType == 28)
	{
		do
		{
			cToken += 4;
		}
		while (list[cToken].tokenType == 17); // keep moving token by 4 while cToken is a comma

		// grab the next token
		cToken++;
	}

	// if cToken is var
	if (list[cToken].tokenType == 29)
	{
		do
		{
			numVars++;
			cToken += 2;
		}
		while (list[cToken].tokenType == 17); // continue to loop while the cToken is a comma

		// grab the next token
		cToken++;
	}

	emit("INC", 6, 0, 0, 3 + numVars, code);
	genStatement(table, list, code);
}
//------------------------------------------------------------------------------
void genStatement(symbol *table, lexeme *list, instruction *code)
{
	int codeIndex = 0;
	int codeIndexForCondition = 0;
	int codeIndexForJump = 0;
	int tableIndex = 0;

	// if the token is an identifier
	if (list[cToken].tokenType == 2)
	{
		// save the location of the identifier in the symbol table
		tableIndex = getTableIndex(table, list[cToken].lexeme);
		// move token over 2 spots
		cToken += 2;
		genExpression(table, list, code, 0);
		emit("STO", 4, 0, 0, table[tableIndex].addr, code); // i think that M comes from the symbol table means table[tabledIndex].addr
	}
	// if the token is begin
	if (list[cToken].tokenType == 21)
	{
		cToken++;
		genStatement(table, list, code);
		while (list[cToken].tokenType == 18) // continue looping while token is ";"
		{
			cToken++;
			genStatement(table, list, code);
		}
		cToken++;
	}
	// if the token is an if
	if (list[cToken].tokenType == 23)
	{
		cToken++;
		genCondition(table, list, code);
		codeIndex = cx;
		emit("JPC", 8, 0, 0, 0, code);
		cToken++;
		genStatement(table, list, code);
		code[codeIndex].m = cx;
	}
	// if the token is a while
	if (list[cToken].tokenType == 25)
	{
		// grab the next token
		cToken++;
		codeIndexForCondition = cx; // save the code index at the conditin
		genCondition(table, list, code);
		cToken++;
		codeIndexForJump = cx; // save the code index at the jump
		emit("JPC", 8, 0, 0, 0, code);
		genStatement(table, list, code);
		emit("JMP", 7, 0, 0, codeIndexForCondition, code);
		code[codeIndexForJump].m = cx;
	}
	// if the token is read
	if (list[cToken].tokenType == 32)
	{
		cToken++;
		tableIndex = getTableIndex(table, list[cToken].lexeme);
		cToken++;
		emit("READ", 9, 0, 0, 2, code);
		emit("STO", 4, 0, 0, table[tableIndex].addr, code);
	}
	// if token is write
	if (list[cToken].tokenType == 31)
	{
		cToken++;
		tableIndex = getTableIndex(table, list[cToken].lexeme);

		// if the identifier is a var
		if (table[tableIndex].kind == 1)
		{
			emit("LOD", 3, 0, 0, table[tableIndex].addr, code);
			emit("WRITE", 9, 0, 0, 1, code);
		}
		// if the identifier is a const
		if (table[tableIndex].kind == 2)
		{
			emit("LIT", 1, 0, 0, table[tableIndex].val, code);
			emit("WRITE", 9, 0, 0, 1, code);
		}
		// grab the next token
		cToken++;
	}
}
//------------------------------------------------------------------------------
void genCondition(symbol *table, lexeme *list, instruction *code)
{
	// if the token is "odd"
	if (list[cToken].tokenType == 8)
	{
		cToken++;
		genExpression(table, list, code, 0);
		emit("ODD", 15, 0, 0, 0, code);
	}
	else
	{
		genExpression(table, list, code, 0);

		// if the token is an "="
		if (list[cToken].tokenType == 9)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("EQL", 17, 0, 0, 1, code);
		}

		// if the token is "<>"
		if (list[cToken].tokenType == 10)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("NEQ", 18, 0, 0, 1, code);
		}

		// if the token is "<"
		if (list[cToken].tokenType == 11)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("LSS", 19, 0, 0, 1, code);
		}

		// if the token is "<="
		if (list[cToken].tokenType == 12)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("LEQ", 20, 0, 0, 1, code);
		}

		// if the token is ">"
		if (list[cToken].tokenType == 13)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("GTR", 21, 0, 0, 1, code);
		}

		// if the token is ">="
		if (list[cToken].tokenType == 14)
		{
			cToken++;
			genExpression(table, list, code, 1);
			emit("GEQ", 22, 0, 0, 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genExpression(symbol *table, lexeme *list, instruction *code, int reg)
{
	// if the token is a "+"
	if (list[cToken].tokenType == 4)
		cToken++;
	// if the token is a "-"
	if (list[cToken].tokenType == 5)
	{
		cToken++;
		genTerm(table, list, code, reg);
		emit("NEG", 10, reg, 0, 0, code);
		// while the token is either a "+" or "-"
		while (list[cToken].tokenType == 5 || list[cToken].tokenType == 4)
		{
			// if the token is a "+"
			if (list[cToken].tokenType == 4)
			{
				cToken++;
				genTerm(table, list, code, reg);
				emit("ADD", 11, reg, reg, reg + 1, code);
			}
			// if the token is a "-"
			if (list[cToken].tokenType == 5)
			{
				cToken++;
				genTerm(table, list, code, reg);
				emit("SUB", 12, reg, reg, reg + 1, code);
			}
		}
		return;
	}

	genTerm(table, list, code, reg);

	// while the toke is "+" or "-"
	while (list[cToken].tokenType == 5 || list[cToken].tokenType == 4)
	{
		// if the token is a "+"
		if (list[cToken].tokenType == 4)
		{
			cToken++;
			genTerm(table, list, code, reg + 1);
			emit("ADD", 11, reg, reg, reg + 1, code);
		}
		// if the token is a "-"
		if (list[cToken].tokenType == 5)
		{
			cToken++;
			genTerm(table, list, code, reg + 1);
			emit("SUB", 12, reg, reg, reg + 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genTerm(symbol *table, lexeme *list, instruction *code, int reg)
{
	genFactor(table, list, code, reg);
	// while the token is "*" or "/"
	while (list[cToken].tokenType == 6 || list[cToken].tokenType == 7)
	{
		// if the token is "*"
		if (list[cToken].tokenType == 6)
		{
			cToken++;
			genFactor(table, list, code, reg + 1);
			emit("MUL", 13, reg, reg, reg + 1, code);
		}
		// if the toke is "/"
		if (list[cToken].tokenType == 7)
		{
			cToken++;
			genFactor(table, list, code, reg + 1);
			emit("DIV", 14, reg, reg, reg + 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genFactor(symbol *table, lexeme *list, instruction *code, int reg)
{
	int tableIndex = 0;
	// if the token is an identifier
	if (list[cToken].tokenType == 2)
	{
		// save the symbol table index
		tableIndex = getTableIndex(table, list[cToken].lexeme);
		// if the token is a const
		if (table[tableIndex].kind == 1)
		{
			emit("LIT", 1, reg, 0, table[tableIndex].val, code);
		}
		// if the token is a var
		if (table[tableIndex].kind == 2)
		{
			emit("LOD", 3, reg, 0, table[tableIndex].addr, code);
		}

		cToken++;
	}
	// else if the token is a number
	else if (list[cToken].tokenType == 3)
	{
		emit("LIT", 1, reg, 0, atoi(list[cToken].lexeme), code);
		cToken++;
	}
	else
	{
		cToken++;
		genExpression(table, list, code, reg);
		cToken++;
	}
}
//------------------------------------------------------------------------------
void emit(char *op, int opcode, int r, int l, int m, instruction *code) {
	if (cx > CODE_SIZE) {
		printf("ERROR: Code exceeds maximum length\n");
		exit(0);
	} else {
		strcpy(code[cx].op, op);
		code[cx].opcode = opcode;
		code[cx].r  = r;
		code[cx].l  = l;
		code[cx].m  = m;
		cx++;
	}
}
//------------------------------------------------------------------------------
// takes in an identifier and finds its symbol table index
int getTableIndex(symbol *table, char *target)
{
	int i;

	for (i = 0; i < tSize; i++)
	{
		if (strcmp(table[i].name, target) == 0)
			return i;
	}
	return 0;
}
