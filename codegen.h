#ifndef __CODEGEN_H
#define __CODEGEN_H

#include "lex.h"
#include "parser.h"

typedef struct instruction
{
	int lineNum;
	int opcode;
	char op[8];
	int r;
	int l;
	int m;
} instruction;

instruction* generateCode(symbol *table, lexeme *list, int tableSize, int listSize, int *codeSize);
void genProgram(symbol *table, lexeme *list, instruction *code);
void genBlock(symbol *table, lexeme *list, instruction *code);
void genStatement(symbol *table, lexeme *list, instruction *code);
void genCondition(symbol *table, lexeme *list, instruction *code);
void genExpression(symbol *table, lexeme *list, instruction *code, int reg);
void genTerm(symbol *table, lexeme *list, instruction *code, int reg);
void genFactor(symbol *table, lexeme *list, instruction *code, int reg);
void emit(char *op, int opcode, int r, int l, int m, instruction *code);
void addLineNum(instruction *code, int codeSize);


#endif
