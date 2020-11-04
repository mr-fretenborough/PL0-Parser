THIS IS THE PSUEDOCODE FOR THE PARSER

this takes is an array of lexemes from Hw2 and outputs a symbol table


PROGRAM
  BLOCK
  if token != .
    error cuz a block is followed by a perdiod

BLOCK
  calls const-declaration
        var-declaration
        statement

CONST-DECLARATION
  if token == const
    do
      get next token
      if token != ident
        error cuz const has to be followed by an identifier
      save ident name
      if the ident name is already in the symbol table then error
      get next token
      if token != =
        error
      get next token
      if token != number then error
      add to the symbol table (kind = 1, saved name, number is current tokek, lexlevel = 0, m = 0, unmarked)
      get next token
    while token == ,
    if token != ;
      error
    get next token

VAR-DECLARATION
if token == var
  numVars = 0
  do
    numVars++
    get next token
    if token != identifier
      error
    if ident is already in the identifier
      error
    add to the symbol table (kind = 2, name is current token, val = 0, lexlevel = 0, m = numVars + 2, unmarked)
    while token == ,
    if token != ;
      error
    get next token
