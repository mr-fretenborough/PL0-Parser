#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include "vm.h"

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

// Init the global stack and dynamic link tracker array
int stack[MAX_STACK_HEIGHT] = {0};
int dynamicLinks[MAX_STACK_HEIGHT] = {0};

// fucntion prototypes
void virtual_machine(instruction *code, int lCode);
int base(int L, int base);
void printExecution(instruction* code, int lCode);

//------------------------------------------------------------------------------
void virtual_machine(instruction *code, int lCode) {
	// initialize variables
	int PC = 0;
	int SP = MAX_STACK_HEIGHT;  // Stack Pointer
	int BP = SP - 1;            // Base Pointer
	int RF[8] = {0};            // register File
	int halt = 1;               // halt flag for the VIM
	int temp;
	instruction IR;

/*
	// print out the generated assembly code
	printf("Line\tOP\tR\tL\tM\n");
	for (int i = 0; i < lCode; i++) {
		printf("%d\t%s\t%d\t%d\t%d\n",
			i, code[i].op, code[i].r, code[i].l, code[i].m);
	}
*/

	// print the initial values
	printf("\n\n\t\t\tpc\tbp\tsp\n");
	printf("Initial Values\t\t%d\t%d\t%d\n", PC, BP, SP);

	// print the value of all the registers
	printf("Registers: ");
	for (int i = 0; i < 8; i++)
	printf("%d ", RF[i]);
	printf("\n");

	// print the contents of the stack
	printf("Stack: \n\n");

	// print labels again
	printf("\t\t\tpc\tbp\tsp\n");

	while (halt == 1) {
		// FETCH CYCLE
		IR = code[PC++];

		// EXECUTION CYCLE
		switch(IR.opcode) {
			case 1:
				RF[IR.r] = IR.m;
				break;
			case 2:
				SP = BP + 1;
				BP = stack[SP - 2];
				PC = stack[SP - 3];
				break;
			case 3:
				RF[IR.r] = stack[base(IR.l, BP) - IR.m];
				break;
			case 4:
				stack[base(IR.l, BP) - IR.m] = RF[IR.r];
				break;
			case 5:
				stack[SP - 1] = base(IR.l, BP); // static link
				stack[SP - 2] = BP;             // dynamic link
				stack[SP - 3] = PC;             // return address
				BP = SP - 1;
				dynamicLinks[BP] = 1;           // save the location of the dynamic link
				PC = IR.m;
				break;
			case 6:
				SP = SP - IR.m;
				break;
			case 7:
				PC = IR.m;
				break;
			case 8:
				if (RF[IR.r] == 0)
					PC = IR.m;
				break;
			case 9:
				switch(IR.m) {
					case 1:
						printf("Register %d: %d\n\n", IR.r, RF[IR.r]);
						break;
					case 2:
						scanf(" %d", &RF[IR.r]);
						break;
					case 3:
						halt = 0;
				}
				break;
			case 10:
				RF[IR.r] *= -1;
				break;
			case 11:
				RF[IR.r] = RF[IR.l] + RF[IR.m];
				break;
			case 12:
				RF[IR.r] = RF[IR.l] - RF[IR.m];
				break;
			case 13:
				RF[IR.r] = RF[IR.l] * RF[IR.m];
				break;
			case 14:
				RF[IR.r] = RF[IR.l] / RF[IR.m];
				break;
			case 15:
				RF[IR.r] = RF[IR.r] % 2;
				break;
			case 16:
				RF[IR.r] = RF[IR.l] % RF[IR.m];
				break;
			case 17:
				RF[IR.r] = (RF[IR.l] == RF[IR.m]);
				break;
			case 18:
				RF[IR.r] = (RF[IR.l] != RF[IR.m]);
				break;
			case 19:
				RF[IR.r] = (RF[IR.l] < RF[IR.m]);
				break;
			case 20:
				RF[IR.r] = (RF[IR.l] <= RF[IR.m]);
				break;
			case 21:
				RF[IR.r] = (RF[IR.l] > RF[IR.m]);
				break;
			case 22:
				RF[IR.r] = (RF[IR.l] >= RF[IR.m]);
				break;
		}

		// print status
		printf("%d %s %d %d %d\t\t%d\t%d\t%d\n",
			IR.lineNum, IR.op, IR.r, IR.l, IR.m, PC, BP, SP);

		// print the value of all the registers
		printf("Registers: ");
		for (int i = 0; i < 8; i++)
			printf("%d ", RF[i]);
		printf("\n");

		// print the contents of the stack
		printf("Stack: ");
		temp = 999;
		// print the stack from bottom up to SP
		while (temp >= SP) {
			// seperates the different activation records
			if (dynamicLinks[temp] == 1)
			printf("| ");

			printf("%d ", stack[temp]);
			temp--;
		}

		printf("\n\n");
	}

}

//------------------------------------------------------------------------------
int base(int L, int base)
{
	int b1 = base;

	while (L > 0) {
		b1 = stack[b1];
		L--;
	}

	return b1;
}

//------------------------------------------------------------------------------
// prints the interpreted assembly code
void printExecution(instruction* code, int lCode) {
	printf("LINE:\tOP:\tR:\tL:\tM:\n");
	for (int i = 0; i < lCode; i++)
	{
		printf("%d\t", i);                // print the line number
		printf("%s\t", code[i].op);   // print the name of the op code
		printf("%d\t", code[i].r);        // print the R value
		printf("%d\t", code[i].l);        // print the L value
		printf("%d\n", code[i].m);        // print the M value
	}
}
//------------------------------------------------------------------------------
