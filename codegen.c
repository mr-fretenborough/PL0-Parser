#include <stdio.h>
#include <stdlib.h>
#include "lex.h"
#include "parser.h"
#include "codegen.h"

#define CODE_SIZE 500

// tSize is the size of the symbol table, lsize is the size of the lexeme list,
// and cx is the current index of the code array
int tSize, lSize, cSize, cx, cToken;

// basic functions
instruction* generateCode (symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize);
void genProgram           (symbol *table, lexeme *list, instruction *code);
void genBlock             (symbol *table, lexeme *list, instruction *code, int lex);
void genStatement         (symbol *table, lexeme *list, instruction *code, int lex);
void genCondition         (symbol *table, lexeme *list, instruction *code, int lex);
void genExpression        (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void genTerm              (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void genFactor            (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void emit                 (char *op, int opcode, int r, int l, int m, instruction *code);
int getTableIndex         (symbol *table, char *target);
void addLineNum           (instruction *code, int codeSize);


//------------------------------------------------------------------------------
instruction* generateCode(symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize)
{
	instruction *code = malloc(CODE_SIZE * sizeof(instruction));

	tSize = tableSize;
	lSize = listSize;
	cSize = &codeSize;
	cToken = 0;
	cx = 0;

	genProgram(table, list, code);

	*codeSize = cx;
	addLineNum(code, cx);
	return code;
}
//------------------------------------------------------------------------------
void genProgram(symbol *table, lexeme *list, instruction *code) {
	int i = 1;
	int qProc = 0;
	// for each sym in sym tbl
	for (int i = 0; i < tSize; i++) {
		// if the sym is a proc, identify
		if (table[i].kind == 30) {
			table[i].val = ++qProc;
			emit("JMP", 7, 0, 0, 0, code);
		}
	}

	genBlock(table, list, code, 0);
	// tbh i dont get this code
	for (i = 0; code[i].opcode == 7; i++) {
		code[i].m = 1; // replace 1 "the m from that proc's sym tbl entry" !
	}

	// for each line of code
	for (i = 0; i < cSize; i++) {
		// if the line makes a proc call
		if (code[i].opcode == 5) {
			// find table[?].kind w value == code[i].m !
			// code[i].m = table[?].addr | the m value from that sym tbl entry !
		}
	}

	emit("HALT", 9, 0, 0, 3, code);

}
//------------------------------------------------------------------------------
void genBlock(symbol *table, lexeme *list, instruction *code, int lex) {
	int numVars = 0;
	int numSyms = 0;

	// if the current token is a const
	if (list[cToken].tokenType == 28) {
		do {
			cToken++;
			numSyms++;
			table[cToken].mark = 0; // unmark
			cToken += 3;
		// keep moving token by 4 while cToken is a comma
		} while (list[cToken].tokenType == 17);

		// grab the next token
		cToken++;
	}

	// if cToken is var
	if (list[cToken].tokenType == 29) {
		do {
			cToken++;
			numVars++;
			numSyms++;
			table[cToken].mark = 0; // unmark
			cToken++;
		// continue to loop while the cToken is a comma
		} while (list[cToken].tokenType == 17);

		// grab the next token
		cToken++;
	}

	// if token is a procedure
	if (list[cToken].tokenType == 30) {
		do {
			cToken++;
			numSyms++;
			table[cToken].mark = 0; // unmark
			cToken += 2;

			genBlock(table, list, code, lex + 1);
			emit("RTN", 2, 0, 0, 0, code);

			cToken++;
		// if following token is a procedure, continue
		} while (list[cToken].tokenType == 30);
	}

	// update the sym tbl entry for this proc / M = current code index !
	// table[cToken].addr = cToken; *not sure
	emit("INC", 6, 0, 0, 3 + numVars, code);
	genStatement(table, list, code, lex);

	// mark the last numSyms number of unmarked syms
	for (int i = 0; i < numSyms; i++) {
		table[cToken - i].mark = 1;
	}
}
//------------------------------------------------------------------------------
void genStatement(symbol *table, lexeme *list, instruction *code, int lex)
{
	int codeIndex = 0;
	int codeIndexForCondition = 0;
	int codeIndexForJump = 0;
	int tableIndex = 0;

	// if the token is an identifier
	if (list[cToken].tokenType == 2)
	{
		// save the location of the identifier in the symbol table
		// tableIndex = getTableIndex(table, list[cToken].lexeme);

		// save sym tbl ind of the var entry unmarked and closest in lex lvl !
		cToken += 2;
		genExpression(table, list, code, 0, lex);
		emit("STO", 4, 0, table[tableIndex].level, table[tableIndex].addr, code);
	}
	// if the token is a call
	if (list[cToken].tokenType == 27) {
		cToken++;
		// save the sym tbl ind of the proc entry unmarked and closest in lex level !
		emit("CAL", 5, 0, table[tableIndex].level, table[tableIndex].addr, code);
		cToken++;
	}
	// if the token is begin
	if (list[cToken].tokenType == 21)
	{
		cToken++;
		genStatement(table, list, code, lex);
		// if the token is ";" then continue looping
		while (list[cToken].tokenType == 18)
		{
			cToken++;
			genStatement(table, list, code, lex);
		}
		cToken++;
	}
	// if the token is an if
	if (list[cToken].tokenType == 23)
	{
		cToken++;
		genCondition(table, list, code, lex);
		// codeIndex = cx; *not noelle
		// save the code index for jpc !
		emit("JPC", 8, 0, 0, 0, code);
		cToken++;
		genStatement(table, list, code, lex);

		// if token is an "else"
		if (list[cToken].tokenType == 33) {
			cToken++;
			// save the current code index for JMP !
			emit("JMP", 7, 0, 0, 0, code);
			// fix the jpc form earlier | savedCodeIndexForJPC.M = currentCodeIndex !
			genStatement(table, list, code, lex);
			// fix the jpc from earlier | savedCodeIndexForJPC.M = currentCodeIndex !
		// if there is no "else" keyword
		} else {
			// fix the jpc from earlier | savedCodeIndexForJPC.M = currentCodeIndex !
		}
	}
	// if the token is a while
	if (list[cToken].tokenType == 25)
	{
		cToken++;
		// save the code index at the condition
		codeIndexForCondition = cx;

		genCondition(table, list, code, lex);

		cToken++;
		// save the code index at the jump
		codeIndexForJump = cx;

		emit("JPC", 8, 0, 0, 0, code);
		genStatement(table, list, code, lex);
		emit("JMP", 7, 0, 0, codeIndexForCondition, code);
		code[codeIndexForJump].m = cx;
	}
	// if the token is read
	if (list[cToken].tokenType == 32)
	{
		cToken++;
		// tableIndex = getTableIndex(table, list[cToken].lexeme); *not noelle
		// save the sym tbl ind of the var entry unmarked and closest to the cur lex lvl !
		cToken++;
		emit("SYS", 9, 0, 0, 2, code);
		emit("STO", 4, 0, table[tableIndex].level, table[tableIndex].addr, code);
	}
	// if token is write
	if (list[cToken].tokenType == 31)
	{
		cToken++;
		genExpression(table, list, code, 0, lex); // pseudocode says nothing about the reg argument !
		emit("WRITE", 9, 0, 0, 1, code);
		cToken++;
	}
}
//------------------------------------------------------------------------------
void genCondition(symbol *table, lexeme *list, instruction *code, int lex)
{
	// if the token is "odd"
	if (list[cToken].tokenType == 8)
	{
		cToken++;
		genExpression(table, list, code, 0, lex);
		emit("ODD", 15, 0, 0, 0, code);
	}
	else
	{
		genExpression(table, list, code, 0, lex);

		// if the token is an "="
		if (list[cToken].tokenType == 9)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("EQL", 17, 0, 0, 1, code);
		}

		// if the token is "<>"
		if (list[cToken].tokenType == 10)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("NEQ", 18, 0, 0, 1, code);
		}

		// if the token is "<"
		if (list[cToken].tokenType == 11)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("LSS", 19, 0, 0, 1, code);
		}

		// if the token is "<="
		if (list[cToken].tokenType == 12)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("LEQ", 20, 0, 0, 1, code);
		}

		// if the token is ">"
		if (list[cToken].tokenType == 13)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("GTR", 21, 0, 0, 1, code);
		}

		// if the token is ">="
		if (list[cToken].tokenType == 14)
		{
			cToken++;
			genExpression(table, list, code, 1, lex);
			emit("GEQ", 22, 0, 0, 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genExpression(symbol *table, lexeme *list, instruction *code, int reg, int lex)
{
	// if the token is a "+"
	if (list[cToken].tokenType == 4)
		cToken++;
	// if the token is a "-"
	if (list[cToken].tokenType == 5)
	{
		cToken++;
		genTerm(table, list, code, reg, lex);
		emit("NEG", 10, reg, 0, 0, code);
		// while the token is either a "+" or "-"
		while (list[cToken].tokenType == 5 || list[cToken].tokenType == 4)
		{
			// if the token is a "+"
			if (list[cToken].tokenType == 4)
			{
				cToken++;
				genTerm(table, list, code, reg, lex);
				emit("ADD", 11, reg, reg, reg + 1, code);
			}
			// if the token is a "-"
			if (list[cToken].tokenType == 5)
			{
				cToken++;
				genTerm(table, list, code, reg, lex);
				emit("SUB", 12, reg, reg, reg + 1, code);
			}
		}
		return;
	}

	genTerm(table, list, code, reg, lex);

	// while the toke is "+" or "-"
	while (list[cToken].tokenType == 5 || list[cToken].tokenType == 4)
	{
		// if the token is a "+"
		if (list[cToken].tokenType == 4)
		{
			cToken++;
			genTerm(table, list, code, reg + 1, lex);
			emit("ADD", 11, reg, reg, reg + 1, code);
		}
		// if the token is a "-"
		if (list[cToken].tokenType == 5)
		{
			cToken++;
			genTerm(table, list, code, reg + 1, lex);
			emit("SUB", 12, reg, reg, reg + 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genTerm(symbol *table, lexeme *list, instruction *code, int reg, int lex)
{
	genFactor(table, list, code, reg, lex);
	// while the token is "*" or "/"
	while (list[cToken].tokenType == 6 || list[cToken].tokenType == 7)
	{
		// if the token is "*"
		if (list[cToken].tokenType == 6)
		{
			cToken++;
			genFactor(table, list, code, reg + 1, lex);
			emit("MUL", 13, reg, reg, reg + 1, code);
		}
		// if the toke is "/"
		if (list[cToken].tokenType == 7)
		{
			cToken++;
			genFactor(table, list, code, reg + 1, lex);
			emit("DIV", 14, reg, reg, reg + 1, code);
		}
	}
}
//------------------------------------------------------------------------------
void genFactor(symbol *table, lexeme *list, instruction *code, int reg, int lex)
{
	int tableIndex = 0;
	// if the token is an identifier
	if (list[cToken].tokenType == 2)
	{
		// save the symbol table index *old
		// tableIndex = getTableIndex(table, list[cToken].lexeme); *old

		// save the sym tbl ind of the var/const entry unmark and closest in lex lexlevel !

		// if the token is a const
		if (table[tableIndex].kind == 1)
		{
			emit("LIT", 1, reg, 0, table[tableIndex].val, code);
		}
		// if the token is a var
		if (table[tableIndex].kind == 2)
		{
			emit("LOD", 3, reg, table[tableIndex].level, table[tableIndex].addr, code);
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
		genExpression(table, list, code, reg, lex);
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
//------------------------------------------------------------------------------
void addLineNum(instruction *code, int codeSize)
{
	int i;
	for (i = 0; i < codeSize; i++)
		code[i].lineNum = i;
}
