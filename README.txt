Kier Fretenborough
Bryce Hitchcock

HOW TO RUN ::
$ gcc driver.c lex.c parser.c codegen.c vm.c
$ ./a.out correctInput.txt -l -a -v

compiler directives:
  -1 to print the lexeme list (HW2)
  -a to print the generated assembly code
  -v to print the virtual machine execution (HW1)

correct input and output are in correctInput.txt and correctOutput.txt respectively

all error inputs and outputs in error list correspond to the error text files with the same number

ERROR LIST ::
1: = must be followed by a number.
2: Identifier must be followed by =.
3: const, var, procedure must be followed by identifier.
4: Semicolon or comma missing.
5: Period expected.
6: Undeclared identifier.
7: Assignment to constant or procedure is not allowed.
8: Assignment operator expected.
9: then expected.
10: do expected.
11: Relational operator expected.
12: Right parenthesis missing.
13: The preceding factor cannot begin with this symbol.
14: Number is too long.
15: Identifier has already been declared.
16: End expected
17: Identifier expected after read
18: Can only read to a var.
19: Identifier expected after write
20: Invalid symbol.
21: Identifier is too long
22: Identifier does not start with a letter
