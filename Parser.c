#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct lexeme {
    char lexeme[100];
    int tokenType;
} lexeme;

int main(int argc, char **argv) {
    // init the file pointer and open file
    FILE *f;
    f = fopen(argv[1], "r");
    if (f == NULL) {
        printf("ERROR: Failed to open file\n");
        return 0;
    }

    return 1;
}