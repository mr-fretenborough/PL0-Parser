// Kier Fretenborough and Bryce Hitchcock
// COP 3402 Computer Architecture
// Homework #1: The P-Machine
// Prof. Mantagne
// 2020 September 24

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STACK_HEIGHT 1000
#define MAX_CODE_LENGTH 500

// define the IR struct
typedef struct {
    int lineNum;
    char OPname[4];
    int OP;
    int R;
    int L;
    int M;
} code;

// Function prototype for base
int base(int L, int base);

// global stack
int stack[MAX_STACK_HEIGHT] = {0};

// keeps track of the dynamic links for the different activation records
int dynamicLinks[MAX_STACK_HEIGHT] = {0}; // 1 if dynamic link, 0 if not

int main(int argc, char **argv)
{
  // declare variables
  int PC = 0;                 // Program Counter
  int SP = MAX_STACK_HEIGHT;  // Stack Pointer
  int BP = SP - 1;            // Base Pointer
  code IR;                    // Instuction Register
  int RF[8] = {0};            // register File
  FILE *ifp;                  // file pointer for the input file
  int numLines = 0;           // num lines in the file
  code *text;                 // to hold an array of instructions
  char numLineFinder;         // just needed a char to look through file
  int halt = 1;               // halt flag for the VIM
  int temp;                   // used to iterate through the stack in execution cycle

  // open file
  ifp = fopen(argv[1], "r");
  if (ifp == NULL)
  {
    printf("Failed to open file");
    return -1;
  }

  // find the number of lines in file
  for (numLineFinder = getc(ifp); numLineFinder != EOF; numLineFinder = getc(ifp))
    if (numLineFinder == '\n')
      numLines++;

  // allocate memory for the number of instructions
  text = malloc(sizeof(code) * numLines);

  // reset the file Pointer
  rewind(ifp);

  // populate the text array
  for (int i = 0; i < numLines; i++)
  {
    fscanf(ifp, "%d %d %d %d", &text[i].OP, &text[i].R, &text[i].L, &text[i].M);
    text[i].lineNum = i;
  }

  // print the interpreted assembly code
  printf("LINE:\tOP:\tR:\tL:\tM:\n");
  for (int i = 0; i < numLines; i++) // loops through every line
  {
    switch(text[i].OP) {
      case 1:
        strcpy(text[i].OPname, "LIT");
        break;
      case 2:
        strcpy(text[i].OPname, "RTN");
        break;
      case 3:
        strcpy(text[i].OPname, "LOD");
        break;
      case 4:
        strcpy(text[i].OPname, "STO");
        break;
      case 5:
        strcpy(text[i].OPname, "CAL");
        break;
      case 6:
        strcpy(text[i].OPname, "INC");
        break;
      case 7:
        strcpy(text[i].OPname, "JMP");
        break;
      case 8:
        strcpy(text[i].OPname, "JPC");
        break;
      case 9:
        strcpy(text[i].OPname, "SYS");
        break;
      case 10:
        strcpy(text[i].OPname, "NEG");
        break;
      case 11:
        strcpy(text[i].OPname, "ADD");
        break;
      case 12:
        strcpy(text[i].OPname, "SUB");
        break;
      case 13:
        strcpy(text[i].OPname, "MUL");
        break;
      case 14:
        strcpy(text[i].OPname, "DIV");
        break;
      case 15:
        strcpy(text[i].OPname, "ODD");
        break;
      case 16:
        strcpy(text[i].OPname, "MOD");
        break;
      case 17:
        strcpy(text[i].OPname, "EQL");
        break;
      case 18:
        strcpy(text[i].OPname, "NEQ");
        break;
      case 19:
        strcpy(text[i].OPname, "LSS");
        break;
      case 20:
        strcpy(text[i].OPname, "LEQ");
        break;
      case 21:
        strcpy(text[i].OPname, "GTR");
        break;
      case 22:
        strcpy(text[i].OPname, "GEQ");
        break;
    }
    printf("%d\t", i);                // print the line number
    printf("%s\t", text[i].OPname);   // print the name of the op code
    printf("%d\t", text[i].R);        // print the R value
    printf("%d\t", text[i].L);        // print the L value
    printf("%d\n", text[i].M);        // print the M value
  }

  // just adding new lines
  printf("\n\n");

  // print the initial values
  printf("\t\t\tpc\tbp\tsp\n");
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

  // begin P-Machine Cycles
  while (halt == 1)
  {
    // fetch Cycle
    IR = text[PC++];

    // Execution Cycle
    switch(IR.OP) {
      case 1:
        RF[IR.R] = IR.M;
        break;
      case 2:
        SP = BP + 1;
        BP = stack[SP - 2];
        PC = stack[SP - 3];
        break;
      case 3:
        RF[IR.R] = stack[base(IR.L, BP) - IR.M];
        break;
      case 4:
        stack[base(IR.L, BP) - IR.M] = RF[IR.R];
        break;
      case 5:
        stack[SP - 1] = base(IR.L, BP); // static link
        stack[SP - 2] = BP;             // dynamic link
        stack[SP - 3] = PC;             // return address
        BP = SP - 1;
        dynamicLinks[BP] = 1;           // save the location of the dynamic link
        PC = IR.M;
        break;
      case 6:
        SP = SP - IR.M;
        break;
      case 7:
        PC = IR.M;
        break;
      case 8:
        if (RF[IR.R] == 0)
            PC = IR.M;
        break;
      case 9:
        switch(IR.M) {
            case 1:
                printf("Register %d: %d\n\n", IR.R, RF[IR.R]);
                break;
            case 2:
                scanf(" %d", &RF[IR.R]);
                break;
            case 3:
                halt = 0;
        }
        break;
      case 10:
        RF[IR.R] *= -1;
        break;
      case 11:
        RF[IR.R] = RF[IR.L] + RF[IR.M];
        break;
      case 12:
        RF[IR.R] = RF[IR.L] - RF[IR.M];
        break;
      case 13:
        RF[IR.R] = RF[IR.L] * RF[IR.M];
        break;
      case 14:
        RF[IR.R] = RF[IR.L] / RF[IR.M];
        break;
      case 15:
        RF[IR.R] = RF[IR.R] % 2;
        break;
      case 16:
        RF[IR.R] = RF[IR.L] % RF[IR.M];
        break;
      case 17:
        RF[IR.R] = (RF[IR.L] == RF[IR.M]);
        break;
      case 18:
        RF[IR.R] = (RF[IR.L] != RF[IR.M]);
        break;
      case 19:
        RF[IR.R] = (RF[IR.L] < RF[IR.M]);
        break;
      case 20:
        RF[IR.R] = (RF[IR.L] <= RF[IR.M]);
        break;
      case 21:
        RF[IR.R] = (RF[IR.L] > RF[IR.M]);
        break;
      case 22:
        RF[IR.R] = (RF[IR.L] >= RF[IR.M]);
        break;
    }

    // print status
    printf("%d %s %d %d %d\t\t%d\t%d\t%d\n", IR.lineNum, IR.OPname, IR.R, IR.L, IR.M, PC, BP, SP);
    // print the value of all the registers
    printf("Registers: ");
    for (int i = 0; i < 8; i++)
      printf("%d ", RF[i]);
    printf("\n");
    // print the contents of the stack
    printf("Stack: ");
    temp = 999;
    while (temp >= SP)  // this prints everything in the stack from the bottom up to SP
    {
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
  int b1;
  b1 = base;
  while (L > 0)
  {
    b1 = stack[b1];
    L--;
  }
  return b1;
}
