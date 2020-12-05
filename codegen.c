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
instruction* generateCode (symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize);
void genProgram           (symbol *table, lexeme *list, instruction *code);
void genBlock             (symbol *table, lexeme *list, instruction *code, int lex, int ProcedureIndex);
void genStatement         (symbol *table, lexeme *list, instruction *code, int lex);
void genCondition         (symbol *table, lexeme *list, instruction *code, int lex);
void genExpression        (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void genTerm              (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void genFactor            (symbol *table, lexeme *list, instruction *code, int reg, int lex);
void emit                 (char *op, int opcode, int r, int l, int m, instruction *code);
int  getTableIndex        (symbol *table, char *target);
void addLineNum           (instruction *code, int codeSize);
int  findProcedureM       (symbol *table, int codeM);
int  findConstantIndex    (symbol* table, char* name, int lex);
int  getSymIndex          (symbol *table, char* name, int type, int lex);
int  findVarIndex         (symbol* table, char* name, int lex);
int  findProcIndex        (symbol* table, char* name, int lex);
int  findUnmarked         (symbol *table, char* name, int lex);


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
	addLineNum(code, cx);
	return code;
}
//------------------------------------------------------------------------------
void genProgram(symbol *table, lexeme *list, instruction *code) {
	int array[CODE_SIZE];

	int i = 1;
	int arrayIndex = 0;
	int qProc = 0;
	int tempProcedureM = 0;
	// for each sym in sym tbl
	for (int i = 0; i < tSize; i++) {
		// if the sym is a proc, identify
		if (table[i].kind == 3) {
			table[i].val = ++qProc;
			emit("JMP", 7, 0, 0, 0, code);
			array[arrayIndex++] = i;
		}
	}

	genBlock(table, list, code, 0, 0);

	arrayIndex = 0;

	for (int i = 0; i < cx; i++) {
		if (code[i].opcode == 7) {
			code[i].m = table[array[arrayIndex++]].addr;
		}
	}


	// for each line of code
	for (i = 0; i < cx; i++) {
		// if the line makes a procedure call
		if (code[i].opcode == 5) {
			 tempProcedureM = findProcedureM(table, code[i].m);
			 code[i].m = tempProcedureM;
		}
	}

	emit("HALT", 9, 0, 0, 3, code);

}
//------------------------------------------------------------------------------
void genBlock(symbol *table, lexeme *list, instruction *code, int lex, int ProcedureIndex) {
	int numVars = 0;
	int numSyms = 0;
	int tempProcedureIndex = 0;

	// if the current token is a const
	if (list[cToken].tokenType == 28) {
		do {
			cToken++;
			numSyms++;
			table[findConstantIndex(table, list[cToken].lexeme, lex)].mark = 0; // unmark
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
			table[findVarIndex(table, list[cToken].lexeme, lex)].mark = 0; // unmark
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
			tempProcedureIndex = findProcIndex(table, list[cToken].lexeme, lex);
			table[tempProcedureIndex].mark = 0; // unmark
			cToken += 2;

			genBlock(table, list, code, lex + 1, tempProcedureIndex);
			emit("RTN", 2, 0, 0, 0, code);

			cToken++;
		// if following token is a procedure, continue
		} while (list[cToken].tokenType == 30);
	}
	table[ProcedureIndex].addr = cx;

	emit("INC", 6, 0, 0, 3 + numVars, code);
	genStatement(table, list, code, lex);

	// mark the last numSyms number of unmarked syms
	markSymbols(list, table, numSyms);
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
		// save sym tbl ind of the var entry unmarked and closest in lex lvl
		tableIndex = findUnmarked(table, list[cToken].lexeme, lex);

		cToken += 2;
		genExpression(table, list, code, 0, lex);

		emit("STO", 4, 0, lex - table[tableIndex].level, table[tableIndex].addr, code); // STO2
	}
	// if the token is a call
	if (list[cToken].tokenType == 27) {
		cToken++;

		// save the sym tbl ind of the proc entry unmarked and closest in lex level
		tableIndex = findUnmarked(table, list[cToken].lexeme, lex);

		emit("CAL", 5, 0, lex - table[tableIndex].level, table[tableIndex].val, code);
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
		codeIndexForCondition = cx;
		emit("JPC", 8, 0, 0, 0, code);
		cToken++;
		genStatement(table, list, code, lex);

		// if token is an "else"
		if (list[cToken].tokenType == 33) {
			cToken++;
			codeIndexForJump = cx;
			emit("JMP", 7, 0, 0, 0, code);
			code[codeIndexForCondition].m = cx;
			genStatement(table, list, code, lex);
			code[codeIndexForJump].m = cx;
		// if there is no "else" keyword
		} else {
			code[codeIndexForCondition].m = cx;
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
		// save the sym tbl ind of the var entry unmarked and closest to the cur lex lvl
		tableIndex = findUnmarked(table, list[cToken].lexeme, lex);

		cToken++;
		emit("SYS", 9, 0, 0, 2, code);
		emit("STO", 4, 0, lex - table[tableIndex].level, table[tableIndex].addr, code); // STO1
	}
	// if token is write
	if (list[cToken].tokenType == 31)
	{
		cToken++;
		genExpression(table, list, code, 0, lex);
		emit("WRITE", 9, 0, 0, 1, code);
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
				genTerm(table, list, code, reg + 1, lex);
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

		// save the sym tbl ind of the var/const entry unmark and closest in lex lvl
		tableIndex = findUnmarked(table, list[cToken].lexeme, lex);

		// if the token is a const
		if (table[tableIndex].kind == 1)
		{
			emit("LIT", 1, reg, 0, table[tableIndex].val, code);
		}
		// if the token is a var
		if (table[tableIndex].kind == 2)
		{
			emit("LOD", 3, reg, lex - table[tableIndex].level, table[tableIndex].addr, code);
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
// this is a modular version of the getVar, getConst, and getProc
int getSymIndex(symbol *table, char* name, int type, int lex)
{
	for (int i = 0; i < tSize; i++) {
		if (strcmp(table[i].name, name) == 0 && table[i].level == lex && table[i].kind == type)
			return i;
	}

	// if at this point, no sym found
	printf("ERROR: Codegen failed to find the symbol (getSym)\n");
	return 0;

}
//------------------------------------------------------------------------------
// takes a token name and lex level and finds unmarked sym + closest in lex
// you want local x, not global x
int findUnmarked(symbol *table, char* name, int lex)
{
	int index = -1;
	int closest = -1;

	for (int i = 0; i < tSize; i++) {
		if (strcmp(table[i].name, name) == 0)
			if (table[i].mark == 0)
				if (table[i].level > closest && table[i].level <= lex) {
					closest = table[i].level;
					index = i;
				}
	}

	// if index/closest remain unchanged, then no symbol found that fits criteria
	if (index == -1 || closest == -1)
		printf("ERROR: Codegen failed to find symbol (findUnmark)\n");

	return index;
}
//------------------------------------------------------------------------------
void addLineNum(instruction *code, int codeSize)
{
	int i;
	for (i = 0; i < codeSize; i++)
		code[i].lineNum = i;
}
//------------------------------------------------------------------------------
int findProcedureM(symbol *table, int codeM)
{
	int i;

	for (i = 0; i < tSize; i++)
		if (table[i].kind == 3 && table[i].val == codeM)
			return table[i].addr;

	// if at this point, throw error bc no address found
	printf("ERROR: Codegen failed to find procedure address (findProcM)\n");
	return 0;
}
//------------------------------------------------------------------------------
int findConstantIndex(symbol* table, char* name, int lex) {
	for (int i = 0; i < tSize; i++) {
		if (strcmp(table[i].name, name) == 0 && table[i].level == lex && table[i].kind == 1)
			return i;
	}

	// if at this point, no sym found
	printf("ERROR: Codegen failed to find the symbol (findConst)\n");
	return 0;
}
//------------------------------------------------------------------------------
int findVarIndex(symbol* table, char* name, int lex) {
	for (int i = 0; i < tSize; i++) {
		if (strcmp(table[i].name, name) == 0 && table[i].level == lex && table[i].kind == 2)
			return i;
	}
	printf("ERROR: Codegen failed to find symbol (findVar)\n");
	return 0;
}
//------------------------------------------------------------------------------
int findProcIndex(symbol* table, char* name, int lex) {
	for (int i = 0; i < tSize; i++) {
		if (strcmp(table[i].name, name) == 0 && table[i].level == lex && table[i].kind == 3)
			return i;
	}
	printf("ERROR: Codegen failed to find symbol (findProc)\n");
	return 0;
}
