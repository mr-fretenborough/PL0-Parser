#include "lex.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

lexeme* lex_analyze(FILE *ifp, int *listCount)
{
	lexeme *list = malloc(sizeof(lexeme) * 1000);
	char tempString[100] = {""}, c;
	*listCount = 0;

	// begin the file parsing
	c = getc(ifp);
	while (c != EOF) {
			// check if letter then add to string
			if (isalpha(c)) {
					addCharacter(tempString, c);
					c = fgetc(ifp);
					// keeps adding new chars to string as long as they are a number or letter
					while (isalpha(c) || isdigit(c)) {
							addCharacter(tempString, c);
							c = fgetc(ifp);
					}
					// if the string is a key word then add to lexeme list as key word
					// otherwise check to see if valid ident
					if (!isKeyWord(tempString, list, listCount) && isIdentifierValid(tempString, list, listCount)) {
							strcpy(list[*listCount].lexeme, tempString);
							list[*listCount].tokenType = 2;
							(*listCount)++;
					}
			// check if digit
			} else if (isdigit(c)) {
					addCharacter(tempString, c);
					c = fgetc(ifp);
					// keep adding new chars until we encounter a non alphanumeric char
					while (isalpha(c) || isdigit(c)) {
							addCharacter(tempString, c);
							c = fgetc(ifp);
					}
					// if number valid, add to lexeme table
					if (isNumberValid(tempString, list, listCount)) {
							strcpy(list[*listCount].lexeme, tempString);
							list[*listCount].tokenType = 3;
							(*listCount)++;
					}
			// if space, skip
			} else if (isspace(c)) {
					c = fgetc(ifp);
			// check if valid special character
			} else {
					switch(c) {
							case ('+'):
									list[*listCount].tokenType = 4;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case ('-'):
									list[*listCount].tokenType = 5;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case ('*'):
									list[*listCount].tokenType = 6;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case ('/'):
									c = fgetc(ifp);
									if (c == '*') {
											while (c != EOF) {
													c = fgetc(ifp);
													if (c == '*') {
															c = fgetc(ifp);
															if (c == '/') {
																	break;
															}
													}
											}
											c = fgetc(ifp);
											break;
									} else {
											list[*listCount].tokenType = 7;
											list[*listCount].lexeme[0] = '/';
											list[*listCount].lexeme[1] = '\0';
											(*listCount)++;
											break;
									}
							case ('('):
									list[*listCount].tokenType = 15;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case (')'):
									list[*listCount].tokenType = 16;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case ('<'):
									list[*listCount].tokenType = 11;
									addCharacter(tempString, c);
									c = fgetc(ifp);
									switch(c) {
											case ('='):
													addCharacter(tempString, c);
													strcpy(list[*listCount].lexeme, tempString);
													list[*listCount].tokenType = 12;
													break;
											case ('>'): // <> means "not equal"
													addCharacter(tempString, c);
													strcpy(list[*listCount].lexeme, tempString);
													list[*listCount].tokenType = 10;
													break;
											default:
													strcpy(list[*listCount].lexeme, tempString);
													break;
									}
									c = fgetc(ifp);
									(*listCount)++;
									break;
							case ('>'):
									list[*listCount].tokenType = 13;
									addCharacter(tempString, c);
									c = fgetc(ifp);
									if (c == '=') {
											addCharacter(tempString, c);
											strcpy(list[*listCount].lexeme, tempString);
											list[*listCount].tokenType = 14;
											c = fgetc(ifp);
									} else {
											strcpy(list[*listCount].lexeme, tempString);
									}
									(*listCount)++;
									break;
							case ('='):
									list[*listCount].tokenType = 9;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case (','):
									list[*listCount].tokenType = 17;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case ('.'):
									list[*listCount].tokenType = 19;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case (';'):
									list[*listCount].tokenType = 18;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
									break;
							case (':'):
									c = fgetc(ifp);
									if (c == '=') {
											list[*listCount].tokenType = 20;
											strcpy(list[*listCount].lexeme, ":=");
											(*listCount)++;
											c = fgetc(ifp);
									} else {
											list[*listCount].tokenType = -4;
											strcpy(list[*listCount].lexeme, ":");
											(*listCount)++;
									}
									break;
							// default case for any symbol not in our language
							default:
									list[*listCount].tokenType = -4;
									list[*listCount].lexeme[0] = c;
									list[*listCount].lexeme[1] = '\0';
									(*listCount)++;
									c = fgetc(ifp);
					}
			}

			// reset the tempstring
			tempString[0] = '\0';
	}
	return list;
}
//------------------------------------------------------------------------------
void printFile(FILE *ifp)
{
    printf("Source Program:\n");
    char c = fgetc(ifp);
    while (c != EOF) {
        printf("%c", c);
        c = fgetc(ifp);
    }
    printf("\n");
    rewind(ifp);
}
//------------------------------------------------------------------------------
int  isIdentifierValid(char *string, lexeme *list, int *counter)
{
    if (strlen(string) > 11) {
        strcpy(list[*counter].lexeme, string);
        list[*counter].tokenType = -1;
        (*counter)++;
        return 0;
    }
    return 1;
}
//------------------------------------------------------------------------------
int isKeyWord(char *string, lexeme *list, int *counter)
{
    int flag = 0;

    if (strcmp(string, "const") == 0) {
        list[*counter].tokenType = 28;
        flag = 1;
    } else if (strcmp(string, "var") == 0) {
        list[*counter].tokenType = 29;
        flag = 1;
    } else if (strcmp(string, "procedure") == 0) {
        list[*counter].tokenType = 30;
        flag = 1;
    } else if (strcmp(string, "call") == 0) {
        list[*counter].tokenType = 27;
        flag = 1;
    } else if (strcmp(string, "begin") == 0) {
        list[*counter].tokenType = 21;
        flag = 1;
    } else if (strcmp(string, "end") == 0) {
        list[*counter].tokenType = 22;
        flag = 1;
    } else if (strcmp(string, "if") == 0) {
        list[*counter].tokenType = 23;
        flag = 1;
    } else if (strcmp(string, "then") == 0) {
        list[*counter].tokenType = 24;
        flag = 1;
    } else if (strcmp(string, "else") == 0) {
        list[*counter].tokenType = 33;
        flag = 1;
    } else if (strcmp(string, "while") == 0) {
        list[*counter].tokenType = 25;
        flag = 1;
    } else if (strcmp(string, "do") == 0) {
        list[*counter].tokenType = 26;
        flag = 1;
    } else if (strcmp(string, "read") == 0) {
        list[*counter].tokenType = 32;
        flag = 1;
    } else if (strcmp(string, "write") == 0) {
        list[*counter].tokenType = 31;
        flag = 1;
    } else if (strcmp(string, "odd") == 0) {
        list[*counter].tokenType = 8;
        flag = 1;
    }

    if (flag) {
        strcpy(list[*counter].lexeme, string);
        (*counter)++;
        return 1;
    } else {
        return 0;
    }
}
//------------------------------------------------------------------------------
void addCharacter(char *string, char c)
{
    int length = (int) strlen(string);
    string[length] = c;
    string[length + 1] = '\0';
}
//------------------------------------------------------------------------------
int isNumberValid(char *string, lexeme *list, int *count)
{
  int i, flag = 0;

  if (!containsLetter(string) && strlen(string) <= 5)
    return 1;
  else
  {
    for (i = 0; i < strlen(string); i++)
    {
      if (i <= 5 && isalpha(string[i]))
      {
        flag = 1;
        break;
      }
    }

    if (flag)
    {
      list[*count].tokenType = -3;
      strcpy(list[*count].lexeme, string);
      (*count)++;
      return 0;
    }
    else
    {
      list[*count].tokenType = -2;
      strcpy(list[*count].lexeme, string);
      (*count)++;
      return 0;
    }
  }
}
//------------------------------------------------------------------------------
int containsLetter(char *string)
{
    int length = (int) strlen(string);
    for (int i = 0; i < length; i++) {
        if (isalpha(string[i]))
            return 1;
    }
    return 0;
}
//------------------------------------------------------------------------------
void printLexemeTable(lexeme *list, int size)
{
    printf("\n");
    printf("Lexeme Table:\n");
    printf("%-16stoken type\n", "lexeme");
    for (int i = 0; i < size; i++) {
        switch (list[i].tokenType) {
            case (-1):
                printf("ERROR: identifier \"%s\" is too long\n", list[i].lexeme);
                break;
            case (-2):
                printf("ERROR: number \"%s\" is too long\n", list[i].lexeme);
                break;
            case (-3):
                printf("ERROR: identifier \"%s\" does not start with a letter\n", list[i].lexeme);
                break;
            case (-4):
                printf("ERROR: invalid symbol \"%s\"\n", list[i].lexeme);
                break;
            default:
                printf("%-16s%d\n", list[i].lexeme, list[i].tokenType);
        }
    }
}
//------------------------------------------------------------------------------
void printLexemeList(lexeme *list, int size)
{
    // print the token types and add ID if type == 2 || 3
    printf("\nLexeme List:\n");
    for (int i = 0; i < size; i++) {
        if (list[i].tokenType >= 0) {
            if (list[i].tokenType == 2 || list[i].tokenType == 3)
                printf("| %d %s ", list[i].tokenType, list[i].lexeme);
            else
                printf("| %d ", list[i].tokenType);
        }
    }
    printf("\n\n");
}
//------------------------------------------------------------------------------
