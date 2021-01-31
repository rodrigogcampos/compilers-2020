/**@<mybc.c>::**/

#include <mybc.h>

int lookahead;
FILE *source;
int main(int argc, char const *argv[])
{
	if (argc > 1)
	{
		source = fopen(argv[1], "r");
		if (source == NULL)
		{
			fprintf(stderr, "%s: cannot open\n",
							argv[1]);
			exit(-1);
		}
	}
	else
	{
		source = stdin;
	}

	lookahead = gettoken(source);

	/** the newst initial symbol is mybc and no longer E **/
	mybc();

	exit(0);
}
