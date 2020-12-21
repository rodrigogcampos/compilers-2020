/**@<parser.h>::**/

/* This is an LL(1)-recursive parser
 *
 * To recall, an LR(1)-grammar for expressions is given by 
 * E -> E oplus T | T
 * T -> T otimes F | F
 * F -> (E) | ID | UINT
 *
 * After removing the left recursion, we find the LL(1)-grammar:
 *
 * E -> T R
 * observing that void E(void) { T(); R() } has the same topology as the production above,
 * we can generalize it to the following productions:
 * R -> oplus T R | <>
 * Analogously :
 	void R(void) {
		if (lookahead == oplus) {
			match (oplus); T(); R();
		} else {
			;
		}
 	}
 * T -> F Q 
 * Q -> otimes F Q | <>
 * F -> (E) | ID | UINT
 * oplus = ['+''-'']
 * otimes = ['*''/']


 * 2020-12-02
 * E -> T R
 * R -> oplus T R | <>
 * Easily we conclude that R => * (oplus T)*. However, we connat mix up regular expression with 
 * grammar.
 * it is introduced the {}-notation to represent Kleene closure:
 * R -> {oplus T}
 * In this case we have simply
 * E
 * T -> F Q 
 * Q -> otimes F Q | <>
 * F -> (E) | ID | UINT
 * oplus = ['+''-'']
 * otimes = ['*''/']
 */

#include <parser.h>

void R(void);
void T(void);
void F(void);
void Q(void);
void match(int expected);
int gettoken(FILE *);
extern int lookahead;
extern FILE *source;


/* FIRST(E)=FIRST(F)=FIRST(F)={'(', OCT, HEX, UINT, ID}
 * FOLLOW(E) = {EOF, ')'}
 * 
 */
void E(void) {
	/* FOLLOW(T) = FIRST(R) - {<>} \cup FOLLOW(E) = {'+', '-', EOF, ')'}*/
	T(); R();
}


//oplus = [+-]
void R(void) {
	if (lookahead == '+' || lookahead == '-') {
		match (lookahead); T(); R();
	} else {
		switch(lookahead) {
			case')':
				break;
			default:
				match(EOF);
		}
		/*NULL string*/;
	}
}

void T(void) {
	F(); Q(); /* FOLLOW(Q) = FOLLOW(T) = {'+', '-', EOF, ')'} */
}

//oplus = [*/]
/* Q -> otimes F Q | <> */
void Q(void) {
	if (lookahead == '*' || lookahead == '/') {
		match (lookahead); F(); Q();
	} else {
		switch(lookahead) {
		case'+':
		case'-':
		case')':
			break;
		default:
			match(EOF);
		}
		/*NULL string*/;
	}
}

/* F -> (E) | ID | UINT */
void F(void) {
	switch(lookahead) {
		case '(': match('('); E(); match(')');
		break;

		case OCT: 
		case HEX: 
		case UINT:
			match(lookahead);
		break;

		default: 
			match(ID);
	}
}

void match(int expected) {
	if (lookahead == expected) {
		lookahead = gettoken(source);
	} else {
		fprintf(stderr, "token mismatch \n");
		exit(-2);
	}
}