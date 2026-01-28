#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "defs.h"
#include "Evaluate.h"
#include "Object.h"
#include "Dictionary.h"

long seed;
Ast* root_ast = NULL;

void usage(const char* program)
{
	fprintf(stderr, "Usage: %s [-p] [-e] [filename]\n", program);
    fprintf(stderr, "  -p: Print Abstract Syntax Tree (AST)\n");
    fprintf(stderr, "  -e: Evaluate (Execute) the program\n");
}

extern FILE *yyin;
int main(int argc, char** argv) {
	int option;
	int print_mode = 0;

	while((option = getopt(argc, argv, "p")) != -1) {
		switch(option) {
			case 'p': print_mode = 1; break;
			default:
				(usage(argv[0]));
				return EXIT_FAILURE;
		}
	}
	if(optind < argc) {
		FILE *fp = fopen(argv[optind], "r");
		if(!fp) {perror("fopen"); return EXIT_FAILURE; }
		yyin = fp;
	}

	linecounter = 1;
	srand(time(NULL));
	seed = rand();

	if (yyparse() == 0) {
		if(root_ast == NULL) {
			fprintf(stderr, "Warnig: Root AST is empty. No statements.\n");
			return EXIT_FAILURE;
		}

		if(print_mode) print(root_ast);
		else {
			int code = evaluate(root_ast);
			fprintf(stderr, "Program end code with %d\n", code);
		}

	}
	return(EXIT_SUCCESS);
}

