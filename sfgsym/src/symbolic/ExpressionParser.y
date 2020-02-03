%{
    #include <iostream>
    #include "sfgsym/config.hpp"
    #include "sfgsym/symbolic/Expression.hpp"

    using namespace std;

    extern int yylex();
    extern int yyparse();
    void yyerror(const char* s);
%}

%union {
    double constant;
    char* variable;
    sfgsym::Expression* node;
}

%type <constant> CONSTANT;
%type <variable> VARIABLE;
%type <node> expr;

%token CONSTANT;

%left '-' '+'
%left '*' '/' '%'
%right '^'

%%
list  : expr ',' expr { cout << "list" << endl; }
      | expr { cout << "list" << endl; }
      ;
expr  : CONSTANT '+' CONSTANT { cout << "expr" << endl; }
      | CONSTANT { cout << "expr" << endl; }
%%
