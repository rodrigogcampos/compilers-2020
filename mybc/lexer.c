/**@<lexer.c>::**/

/* This isthe first example of a lexical analyser 
 * inpired by an NFA 
 * 
 * It is necessary to implement a method to ignore spaces, namely skipspaces
 */
#include <tokens.h>
#include <lexer.h> // last need be last due to inherantece from another files

void skipspaces(FILE *tape) {
	int head;

	while(isspace(head = getc(tape)));

	ungetc(head, tape);

}

/* Next step resides in defining a recognizer for 
 * Pascal'like identifiers 
 *
 * REGEX: ID = [A-Za-z][A-Za-z0-9]*

 isaplha(x) = 1, iff x \in [A-Za-z]
 			= 0, iff x \notin [A-Za-z]
 */
int isID(FILE *tape) {
	int head;

	if (isalpha(head = getc(tape))) {

		while(isalnum(head = getc(tape)));

		ungetc(head, tape);
		return ID;
	}

	ungetc(head, tape);

	return 0;
}

/* Next we have to define the recognizer for
 * simple decimal pattern (unsigned int) 
 * 
 * REGEX: UINT = [1-9][0-9]* | 0
 */
int isUINT (FILE *tape) {
	int head;

	if (isdigit(head = getc(tape))) {
		if (head != '0') {

			while (isdigit(head = getc(tape)));

			ungetc(head, tape);
		}

		return UINT;
	}

	ungetc(head, tape);

	return 0;
}

/* Octal Pattern
 * REGEX: 0[0-7]+
 */
int isOCT(FILE *tape) {
	int head;

	if ((head = getc(tape)) == '0') {
		if (isdigit(head = getc(tape)) && (head < '8')) {

			while (isdigit(head = getc(tape)) && (head < '8'));
			ungetc(head, tape);

			return OCT;
		}

		ungetc(head, tape);

		ungetc('0', tape);

		return 0;
	}

	ungetc(head, tape);
	
	return 0;

}

/* Hexadecimal Pattern
 * REGEX: 0[xX][0-9A-Fa-f]
 */
int isHEX(FILE *tape) {
	return 0;
}

int gettoken (FILE *source) {
	int token;

	skipspaces(source);

	if ((token = isID(source))) { // 2 parentheses to avoid warning from C
		return token;
	}

	if ((token = isOCT(source))) { // 2 parentheses to avoid warning from C
		return token;
	}

	if ((token = isHEX(source))) { // 2 parentheses to avoid warning from C
		return token;
	}
	
	if ((token = isUINT(source))) { // 2 parentheses to avoid warning from C
		return token;
	}

	token = getc(source);

	return token;
}
