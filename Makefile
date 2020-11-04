# to run in terminal $ make

CC = gcc
OPT = -O3
#OPT = -g
WARN = -Wall
CFLAGS = $(OPT) $(WARN) $(INC) $(LIB)

# List all your .c (or .cpp or .cc) files here (source files, excluding header files)
SRC_CODE = driver.c lex.c parser.c codegen.c vm.c

# List corresponding compiled object files here (.o files)
SRC_OBJ = 
 
#################################

# default rule

all: src_prog
	@echo "my work is done here..."


# rule for making src_prog

src_prog: $(SRC_OBJ)
	$(CC) -o src_prog $(CFLAGS) $(SRC_OBJ) -lm
	@echo "-----------DONE WITH SRC_PROG-----------"


# generic rule for converting any .c file to any .o file
 
.c.o:
	$(CC) $(CFLAGS)  -c $*.c


# type "make clean" to remove all .o files plus the src_prog binary

clean:
	rm -f *.o src_prog


# type "make clobber" to remove all .o files (leaves src_prog binary)

clobber:
	rm -f *.o


