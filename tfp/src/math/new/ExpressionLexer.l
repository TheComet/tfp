%{
    #include "tfp/math/new/Expression.hpp"
    #include "tfp/math/new/ExpressionParser.hpp"
    
    #define YY_INPUT(buf, result, buflen) (result = tfpnew::Expression::yyInput(buf, buflen))
%}

%option noyywrap

%%
[A-Za-z_]+([0-9]+)? {
                        yylval.variable = yytext;
                        return VARIABLE;
                    }
-?[0-9]+            {
                        yylval.constant = atoi(yytext);
                        return CONSTANT;
                    }
[-()*/+\n^%]        return *yytext;
[ \t]+              /* ignore whitespace */
.                   yyerror("Invalid character");
%%
