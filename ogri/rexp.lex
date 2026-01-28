%{
int linecounter = 1;
int altindstack[100] = {0};
int sp = 0;
int pendin = 0;
int atbol = 1;
int current_indent = 0;
int fstring_flag = 0;
#define YY_DECL int raw_lexer(void)
%}
%x INDENT_STATE
%x FSTR
%option nounput
%option yylineno
%%
<INITIAL>^\n           { linecounter++; /* 空行は無視 */ }
<INITIAL>^[ \t]*\n     { linecounter++; /* 空行は無視 */ }
"\n"                   { linecounter++; BEGIN(INDENT_STATE); current_indent = 0; return NEWLINE; }
"\r\n"                 { linecounter++; BEGIN(INDENT_STATE); current_indent = 0; return NEWLINE; }
<INDENT_STATE>" "      { current_indent++; }
<INDENT_STATE>"\t"     { current_indent += 8; }
<INDENT_STATE>"\n"     { linecounter++; current_indent = 0; /* 空行だった場合はリセットして継続 */ }
<INDENT_STATE>.        {
    /* 空白以外の文字が出てきたら、INITIALに戻してインデント判定を行う */
    yyless(0); 
    BEGIN(INITIAL);

    if (current_indent > altindstack[sp]) {
        altindstack[++sp] = current_indent;
        return INDENT;
    } else if (current_indent < altindstack[sp]) {
		int count = 0;
        while (sp > 0 && altindstack[sp] > current_indent) {
            sp--;
            count++;
        }
        if (altindstack[sp] != current_indent) {
            yyerror("Indentation error");
        }
        if (count > 0) {
            pendin = count - 1; 
            return DEDENT;
        }
    }
}
is[ \t]+same[ \t]+as 			{ return EQUAL; }
is[ \t]+not[ \t]+same[ \t]+as	{return NEQUAL; }
is[ \t]+less[ \t]+than			{ return LESS; }
is[ \t]+larger[ \t]+than		{ return LARGE; }
is[ \t]+less					{ return NLARGE; }
is[ \t]+larger					{ return NLESS; }
plus							{ return ADD; }
minus							{ return SUBTRACT; }
times							{ return MULTIPLY; }
divided[ \t]+by					{ return DIVIDE; }
remainder						{ return REMAINDER; }
is                      { return IS; }
are						{ return ARE;}
of						{ return OF; }
at								{ return AT; }
from							{ return FROM; }
to								{ return TO; }
end								{ return END; }
when					{ return WHEN; }
otherwise				{ return OTHERWISE; }
repeat					{ return REPEAT; }
until					{ return UNTIL; }
foreach					{ return FOREACH; }
define					{ return DEFINE; }
using					{ return USING; }
that					{ return THAT; }
return					{ return RETURN; }
with					{ return WITH; }
break					{ return BREAK; }
continue				{ return CONTINUE; }
and						{ return AND; }
or						{ return OR; }
not						{ return NOT; }
f"\""					{ BEGIN(FSTR); fstring_flag = 1; return F_OPEN; }
<FSTR>"["				{ BEGIN(INITIAL); return L_BRACKET; }
<FSTR>"]"				{ return R_BRACKET; }
<FSTR>"\""				{ BEGIN(INITIAL); fstring_flag = 0; return F_CLOSE; }
<FSTR>[^"\[\]\n]+			{ yylval = ast_fstring_text(yytext, yylineno); return FSTRING_TEXT; }
"]"						{ if(fstring_flag) BEGIN(FSTR); return R_BRACKET; }
[a-zA-Z_][a-zA-Z0-9_]*  { yylval = ast_identifier(yytext, yylineno); return IDENTIFIER; }
[0-9]+                  { return INTEGER; }
[0-9]*"."[0-9]+         { return REAL; }
\"[^\"\n]*\"			{ char* content = strndup(yytext + 1, strlen(yytext) - 2); yylval = ast_string(content, yylineno); return STRING; }
"//".*                  { }
","                     { return COMMA;}
"."                     { return PERIOD; }
"+"                     { return(ADD); }
"-"                     { return(SUBTRACT); }
"*"                     { return(MULTIPLY); }
"/"                     { return(DIVIDE); }
"%"                     { return REMAINDER; }
"=="					{ return EQUAL; }
"!="					{ return NEQUAL; }
"<="					{ return NLARGE; }
">="					{ return NLESS; }
"<"						{ return LESS; }
">"						{ return LARGE; }
"("                     { return L_PAR; }
")"                     { return R_PAR; }
" "                     { }
"/*"                    { comment(); }
<<EOF>>					{
	return 0;
}
.                       { return UNKNOWN; }
%%
int yywrap(void) {
	return(1);
}
void comment(void) {
	int boolean, first, second;

	boolean = TRUE;
	first = input();
	while (first != EOF && boolean) {
		second = input();
		if (first == '*' && second == '/') {
			boolean = FALSE;
		} else if (first == '\r' && second == '\n') {
			linecounter++;
			first = input();
		} else {
			if (first == '\r' || first == '\n') {
				linecounter++;
			}
			first = second;
		}
	}
	if (first == EOF) {
		fprintf(stderr, "eof in comment\n");
	}
}

