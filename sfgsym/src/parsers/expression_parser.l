%{
    #define YYSTYPE SFGSYMSTYPE
    #define YYLTYPE SFGSYMLTYPE

    #include "sfgsym/parsers/expression_parser.l.h"
    #include "sfgsym/parsers/expression_parser.y.h"
%}

%option nodefault
%option noyywrap
%option reentrant
%option bison-locations
%option nounistd
%option extra-type="struct sfgsym_expr_parser*"
%option prefix="sfgsym"

DOUBLE_EXP      [eE][\+-]?[0-9]+
DOUBLE1         [0-9]+\.[0-9]+?
DOUBLE2         \.[0-9]+
DOUBLE3         [0-9]+\.[0-9]+?{DOUBLE_EXP}?
DOUBLE4         \.[0-9]+{DOUBLE_EXP}?
DOUBLE5         [0-9]+{DOUBLE_EXP}
DOUBLE          {DOUBLE1}|{DOUBLE2}|{DOUBLE3}|{DOUBLE4}|{DOUBLE5}
SYMBOL          [a-zA-Z_][a-zA-Z0-9_]+?

%%
{DOUBLE} {
    yylval->literal = (sfgsym_real)atof(yytext);
    return TOK_LITERAL;
}

[0-9]+ {
    yylval->literal = (sfgsym_real)atoi(yytext);
    return TOK_LITERAL;
}

oo { return TOK_INFINITY; }

[()*/+-^,] { return *yytext; }

[a-zA-Z_][a-zA-Z0-9_]+? {
    yylval->variable = malloc(strlen(yytext) + 1);
    if (yylval->variable == NULL)
        return TOK_ERROR;
    strcpy(yylval->variable, yytext);
    return TOK_VARIABLE;
}

[ \t] { /* ignore whitespace */ }

\n { return TOK_ERROR; }
.  { return TOK_ERROR; }
%%

