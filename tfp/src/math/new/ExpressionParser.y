%{
    #include <iostream>
    #include "tfp/config.hpp"
    #include "tfp/math/new/Expression.hpp"
    
    using namespace std;
    
    extern int yylex();
    extern int yyparse();
    void yyerror(const char* s);
%}

%union {
    double constant;
    char* variable;
    Expression* node;
}

%type <constant> CONSTANT;
%type <variable> VARIABLE;
%type <node> expr;

%token CONSTANT VARIABLE;

%left '-' '+'
%left '*' '/' '%'
%right '^'

%%
list  : expr TOK_SEP expr { cout << "list" << endl; }
      | expr { cout << "list" << endl; }
      ;
expr  : term TOK_INFIX_ADD term { cout << "expr" << endl; }
      | term { cout << "expr" << endl; }
      ;
term  : factor TOK_INFIX_FACTOR factor { cout << "term" << endl; }
      | factor { cout << "term" << endl; }
      ;
factor: power TOK_INFIX_POWER power { cout << "factor" << endl; }
      | power { cout << "factor" << endl; }
      ;
power : TOK_INFIX_ADD base { cout << "power" << endl; }
      | base { cout << "power" << endl; }
      ;
base  : TOK_CONSTANT { cout << "base: " << yylval.constant << endl; }
      | TOK_VARIABLE { cout << "base: " << yylval.variable << endl; }
      | TOK_VARIABLE TOK_OPEN list TOK_CLOSE { cout << "base" << endl; }
      ;
%%
