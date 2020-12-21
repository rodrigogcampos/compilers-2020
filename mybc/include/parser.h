/**@<parser.h>::**/
#include <stdlib.h>
#include <stdio.h>
#include <tokens.h>

void R(void);
void T(void);
void F(void);
void Q(void);
void match(int expected);
int gettoken(FILE *);
extern int lookahead;
extern FILE *source;
