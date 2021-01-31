/**@<lexer.c>::**/

/* This is the first example of a lexical analyser
 * inspired by an NFA
 *
 * It is necessary to implement a method to ignore spaces,
 * namely skipspaces
 */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <tokens.h>
#include <constants.h>
#include <keywords.h>
#include <lexer.h>

void skipspaces(FILE *tape)
{
	int head;

	while (isspace(head = getc(tape))) {
		if (head == '\n') break;
	}

	ungetc(head, tape);
}

/* Next step resides in defining a recognizer for
 * Pascal'like identifiers
 *
 * REGEX: ID = [A-Za-z][A-Za-z0-9]*
 *
 * isalpha(x) = 1, iff x \in [A-Za-z]
 *            = 0, iff x \notin [A-Za-z]
 */

char lexeme[MAXIDLEN + 1];
int isID(FILE *tape)
{
	int i = 0;

	if (isalpha(lexeme[i] = getc(tape))) {
		i++;

		while (isalnum(lexeme[i] = getc(tape))) {
			i++;
		}

		ungetc(lexeme[i], tape);

		lexeme[i] = 0;
		int key = iskeyword(lexeme);
		if(key) return key;

		return ID;
	}

	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

/* Next we have to define the recognizer for
 * simple decimal pattern (unsigned int)
 *
 * REGEX: UINT = [1-9][0-9]*  |  0
 */
int isUINT(FILE *tape)
{
	int i = 0;

	if (isdigit(lexeme[i] = getc(tape))) {
		i++;

		if (lexeme[i] == '0') {
			i++;
			lexeme[i] = 0;
		} else {
			i++;
			while (isdigit(lexeme[i] = getc(tape))) {
				i++;
			}

			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
		return UINT;
	}
	ungetc(lexeme[i], tape);
	return 0;
}
/* NUM is the generic decimal pattern, which means it
 * can be an unsigned interger or a floating-point.
 *
 * UINT = [1-9][0-9]*  |  0
 *
 * FLOAT = ( UINT '.' [0-9]* | '.' [0-9]+ ) eE? | UINT eE
 *
 * eE = [eE] ['+''-']? [0-9]+
 */
int isNUM(FILE *tape) {
	int token = 0, i = 0;

	if((token = isUINT(tape))) {
		i += strlen(lexeme);

		if ((lexeme[i] = getc(tape)) == '.') {
			i++;
			token = FLOAT;
			while (isdigit(lexeme[i] = getc(tape))) {
				i++;
			}
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		} else {
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		}
	} else if((lexeme[i] = getc(tape)) == '.') {
		i++;
		/* Decimal point has been already read */
		if(isdigit(lexeme[i] = getc(tape))) {
			i++;
			token = FLOAT;
			while (isdigit(lexeme[i] = getc(tape))) {
				i++;
			}
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
		} else {
			/* After decimal point another char, non-digit has been read */
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			ungetc(' ', tape);
		}
	} else {
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
	}

	int eE;

	if(token > 0) {
		if(toupper(lexeme[i] = eE = getc(tape)) == 'E') {
			i++;
			int plusminus;
			if((lexeme[i] = plusminus = getc(tape)) == '+' || plusminus == '-') {
				i++;
			} else {
				ungetc(plusminus, tape);
				plusminus = 0;
			}

			if(isdigit(lexeme[i] = getc(tape))) {
				i++;
				token = FLOAT;
				while(isdigit(lexeme[i] = getc(tape)) ) {
					i++;
				}
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
			} else {
				ungetc(lexeme[i], tape);
				i--;
				if (plusminus) {
					i--;
					ungetc(plusminus, tape);
				}
				i--;
				ungetc(eE, tape);
			}
		} else {
			ungetc(eE, tape);
		}
	}
	lexeme[i] = 0;
	return token;
}

/* Octal pattern:
 * REGEX: 0[0-7]+
 */
int isOCT(FILE *tape) {
	int i = 0;

	if ((lexeme[i] = getc(tape)) == '0') {
		i++;
		if(isdigit(lexeme[i] = getc(tape)) && lexeme[i] <= 7) {
			i++;
			while (isdigit(lexeme[i] = getc(tape)) && lexeme[i] <= 7) {
				i++;
			}
			ungetc(lexeme[i], tape);
			lexeme[i] = 0;
			return OCT;
		} else {
			ungetc(lexeme[i], tape);
			i--;
			ungetc('0', tape);
			lexeme[i] = 0;
			return 0;
		}
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

/* Hexadecimal pattern:
 * REGEX: 0[xX][0-9A-Fa-f]+
 */
int isHEX(FILE *tape) {
	int i = 0;

	if ((lexeme[i] = getc (tape)) == '0') {
		i++;
		if (toupper(lexeme[i] = getc(tape)) == 'X') {
			i++;
			if (isxdigit(lexeme[i] = getc(tape))) {
				i++;
				while (isxdigit(lexeme[i] = getc(tape))) {
					i++;
				}
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return HEX;
			} else {
				ungetc(lexeme[i], tape);
				i--;
				ungetc(lexeme[i], tape);
				i--;
				ungetc(lexeme[i], tape);
				lexeme[i] = 0;
				return 0;
			}
		}
		ungetc(lexeme[i], tape);
		ungetc('0', tape);
		return 0;
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

int gettoken(FILE *source)
{
	int token;

	skipspaces(source);

	if ((token = isID(source)))
		return token;

	if ((token = isOCT(source)))
		return token;

	if ((token = isHEX(source)))
		return token;

	if ((token = isNUM(source)))
		return token;

	token = getc(source);

	return token;
}
