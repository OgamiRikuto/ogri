#ifndef __DEFS_H__
#define __DEFS_H__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/Ast.h"

#define TRUE 1
#define FALSE 0

extern char *yytext;
extern int linecounter;
extern int yylineno;

int main(int, char**);
int yylex(void);
void comment(void);
int yyparse(void);
void yyerror(char*);

extern Ast* root_ast;

#endif
