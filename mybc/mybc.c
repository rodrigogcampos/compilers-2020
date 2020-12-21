/**@<mybc.c>::**/
#include <mybc.h>

int lookahead;
FILE *source;
int main(int argc, char const *argv[]) {
	if (argc > 1) {
		source = fopen(argv[1], "r");

		if (source == NULL) {
			fprintf(stderr, "%s: cannot open\n", argv[1]);
			exit(-1);
		} 
	} else {
		source = stdin;
	}

	while ((lookahead = gettoken(source)) != EOF) {
		switch (lookahead) {
			case ID: 
				printf("token = ID\n");
				break;
			case UINT: 
				printf("token = UINT\n");
				break;
			case OCT: 
				printf("token = OCT\n");
				break;
			case HEX: 
				printf("token = HEX\n");
				break;
			default:
				printf("invalid token = %d\n", lookahead);
		}
	}

	exit(0);
}
