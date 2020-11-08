#ifndef __VM_H
#define __VM_H

#include "codegen.h"

void virtual_machine(instruction *code, int lCode);
int base(int L, int base);
void printExecution(instruction* code, int lCode);

#endif
