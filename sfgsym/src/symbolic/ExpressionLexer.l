%{
    #include "sfgsym/symbolic/Expression.hpp"
    #include "sfgsym/symbolic/ExpressionParser.hpp"

    #define YY_INPUT(buf, result, buflen) (result = sfgsym::Expression::yyInput(buf, buflen))
%}

%option noyywrap

%%
-?[0-9]+            {
                        yylval.constant = atoi(yytext);
                        return CONSTANT;
                    }
[-()*/+\n^%]        return *yytext;
[ \t]+              /* ignore whitespace */
%%
