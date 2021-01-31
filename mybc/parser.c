/**@<parser.c>::**/

/** Atividade 2020/12/16: improvisar um tradutor de expressões infixas para pós fixas
 *
 * Spoiler da teoria:
 *
 * Gramática aumentada por regras semânticas de tradução/interpretação
 *
 * Tomemeos a nossa bem conhecida gramática LR(1) [Aula assíncrona 06 próxima]:
 *
 * E -> E (+) T
 * E -> T
 * T -> T (x) F
 * T -> F
 * F -> ( E )
 * F -> v
 * F -> n
 *
 * Onde são definidas as REGEX:
 * v = ID
 * n = UINT | FLOAT | OCT | HEX
 * (+) = '+' | '-'
 * (x) = '*' | '/'
 *
 * Se quisermos interpretar computacionalmente (sistema de execução)
 * a gramática acima, teremos que enxergar seus atributos semânticos (valores)
 * na forma pós fixa:
 *
 * Regras de produção ||      Regras semânticas
 * -------------------||----------------------------
 * E -> E¹ (+) T      || E.t <- E¹.t // T.t // (+).t
 * E -> T             || E.t <- T.t
 * T -> T¹ (x) F      || T.t <- T¹.t // F.t // (x).t
 * T -> F             || T.t <- F.t
 * F -> ( E )         || F.t <- E.t
 * F -> v             || F.t <- v.t
 * F -> n             || F.t <- n.t
 *
 * onde as variáveis são os símbolos de gramática, que são vistas,
 * por sua vez, como pertencentes a tipos estruturados agregados,
 * sendo o campo .t o atributo de tradução (string) ou de interpretação
 * (comando).
 *
 * Por exemplo, e expressão de entrada "2 * a" pode ser tokenizada
 * como n¹ (x) v², onde n¹.t = "2", (x) = "*", v².t = "a". Neste caso,
 * E => T         || E.t <- T.t
 * T => T¹ (x) F² || T.t <- T¹.t // F².t // (x).t
 * T¹ => F¹       || T¹.t <- F¹.t
 * F¹ => n¹       || F¹.t <- n¹.t = "2"
 * F² => v²       || F².t <- v².t = "a"
 * Assim,
 * T => T¹ (x) F² || T.t <- T¹.t // F².t // (x).t = "2"//"a"//"*" = "2a*"
 * visto que T¹.t <- F¹.t = "2"
 *
 *
 * Complicado? Não se vocês tiverem ciência de que o significado de ->
 * e o de <- são exatamente opostos. Um significa dedução, produção, substituição
 * e é usado na BNF, ou na EBNF também. O outro, <-, é bem menos abstrato e significa
 * o operador atribuição da string da direita ao campo (atributo) associado ao símbolo
 * da gramática. Por exemplo, n.t = "2" significa que "2" foi reconhecido como o token
 * numérico, n, mas seu valor léxico "2" foi devidamente armazenado como atributo desse token.
 *
 * Na prática, especialmente para essa gramática, as regras semânticas podem ser vistas como
 * a camada mais abstrata daquilo que se espera na linguagem objeto, conforme a gramática
 * que deriva a linguagem fonte. Ademais, não fica claro em que momento os atributos são
 * sintetizados ao longo da análise sintática, ou do processo de derivação.
 *
 * Aí introduzimos o conceito de gramática de ações semânticas, que uma outra forma de
 * gramática anotada mas que deixa mais explícito como os atributos são sintetizados.
 *
 * No exemplo da gramática LR(1) de expressões, temos a seguinte representação:
 *
 * E -> E <<(+).t <- LEXEME( (+) )>> (+) T <<imprima (+).t>>  |  T
 * T -> T <<(x).t <- LEXEME( (x) )>> (x) F <<imprima (x).t>>  |  F
 * F -> ( E )
 * F -> <<imprima LEXEME(v)>> v
 * F -> <<imprima LEXEME(n)>> n
 *
 * LEXEME é uma função, na prática uma string carregada durante a análise léxica, que retorna
 * o conteúdo léxico do token reconhecido. Assim, a variável "abacaxi123" será reconhecida como
 * ID, mas seu lexema será LEXEME(ID) = "abacaxi123". Portanto, LEXEME só funciona sobre tokens.
 *
 * Como ficam então os atributos de E, T e F? Não são necessários uma vez que cada não terminal
 * é a raiz de uma subárvore que sempre finalizará numa sequência de tokens, deixando esses,
 * conforme a regra de tradução (no caso pós fixa), um rastro no buffer de saída, que pode ser uma
 * variável string que receberá a concatenação dos pedaços (traduções parciais) conforme a regra
 * semântica original.
 *
 * Vamos à pratica com o seguinte diagrama sintático transliterado para um analisador sintático
 * LL(1) recursivo:
 */

#include <keywords.h>
#include <constants.h>
#include <string.h>
#include <parser.h>

extern char lexeme[];
int error = 0;
double E(void);
double T(void);
double F(void);
/**@<parser.c>::**/

/**/ double E_val;/**/

void cmd(void) {
	switch (lookahead) {
	case ';':
	case '\n':
	case EOF:
		break;
	case QUIT:
	case EXIT:
		exit(0);

	default:

			/**/ E_val = /**/E();
		if (error == 0) {
			/**/ printf("%lg\n", E_val);/**/
		}
		// printf("error: %d\n", error);
		error = 0;
	}
}

/*
 mybc -> cms {cmdsep cmd} EOF
 cmd -> [E | quit | exit]
 cmdsep = ';' | '\n'
 */
void mybc(void)
{
	cmd();
	while (lookahead == ';' || lookahead == '\n') {
		match(lookahead);
		cmd();
	}
	match(EOF);
}

/* E -> ['+''-'] T {(+) T} */
double E(void)
{
	/**/int signal = 0; double E_val, T_val;/**/
	if (lookahead == '+' || lookahead == '-' ) {
		/**/signal = lookahead;
		match(lookahead);
	}

	/* FOLLOW(T) = FIRST(R) - {<>} \cup FOLLOW(E) = {'+', '-', EOF, ')'} */
	/**/T_val = /**/ T();
	/**/if(signal == '-'){T_val = - T_val;}
	/**/E_val = T_val; /**/

	while (lookahead == '+' || lookahead == '-') {
		/**/int oplus = lookahead;/**/
		match(lookahead);
		/**/T_val = /**/ T();
		/**/
		if (oplus == '+')	{
			E_val += T_val;
		} else {
			E_val -= T_val;
		}
	}
	/**/ return E_val;/**/
}

/* T -> F {(*) F}*/
double T(void)
{
	/**/double T_val, F_val;/**/
	/**/T_val = /**/ F();

	while (lookahead == '*' || lookahead == '/') {
		/**/int otimes = lookahead;/**/
		match (lookahead); /**/F_val = /**/ F();
		/**/
		if (otimes == '*') {
			T_val *= F_val;
		} else {
			if (F_val == 0) {
				fprintf(stderr, "Math error: Division by zero not allowed\n");
				error = -1;
			} else {
				T_val /= F_val;
			}
		}
		/**/
	}
	/**/return T_val;/**/
}



double memory[MAXTBSIZE];
typedef struct _symtab_ {
	char name[MAXIDLEN+1];
	int pos;
} SYMTAB;
SYMTAB symtab[MAXTBSIZE];
int symtab_nextentry = 0;

double recall(const char *name) {
	int i;

	for(i = 0; i < symtab_nextentry; i++) {
		if(strcmp(name, symtab[i].name) == 0) {
			return memory[i];
		}
	}

	strcpy(symtab[i].name, name);
	symtab_nextentry++;
	return 0;
}

void store(const char *name, double val) {
	int i;

	for (i = 0; i < symtab_nextentry; i++) {
		if (strcmp(name, symtab[i].name) == 0) {
			break;
		}
	}

	if( i == symtab_nextentry) {
		strcpy(symtab[i].name, name);
		symtab_nextentry++;
	}
	memory[i] = val;
}
/* F -> ( E )
 *     | n
 *     | v [ = E ]
 */
double F(void)
{
	/**/double F_val; char name[MAXIDLEN+1];/**/
	switch (lookahead) {
		case '(':
			match('(');
			/**/F_val = /**/E();
			match(')');
			break;

		case UINT:
		case FLOAT:
			/**/F_val = atof(lexeme);/**/
			match(lookahead);
			break;
		case OCT:
			/**/ F_val = strtol(lexeme, NULL, 8); /**/
			match(lookahead);
			break;
		case HEX:
			/**/ F_val = strtol(lexeme, NULL, 16); /**/
			match(lookahead);
			break;
		default:
		  /**/strcpy(name, lexeme);/**/
			match(ID);
			if (lookahead == '=') {
				/*** L-VALUE ***/
				match('='); /**/ F_val = /**/ E();
				/**/store(name, F_val); /**/
			} else {
				/*** R-VALUE ***/
				/**/F_val = recall(name);/**/
				;
			}
	}
	/**/ return F_val; /**/
}

void match(int expected)
{
	if (lookahead == expected)
	{
		lookahead = gettoken(source);
	}
	else
	{
		fprintf(stderr, "token mismatch: expected %d whereas found %d\n",
			expected, lookahead);
			error = -1;
		// exit(-2);
	}
}
