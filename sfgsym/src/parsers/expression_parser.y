%require "3.5"
%code top
{
    #include "sfgsym/parsers/expression_parser.y.h"
    #include "sfgsym/parsers/expression_parser.l.h"
    #include "sfgsym/parsers/expression_parser.h"
    #include "sfgsym/symbolic/expression.h"

    #define driver ((struct sfgsym_expr_parser*)sfgsymget_extra(scanner))

    void sfgsymerror(SFGSYMLTYPE* locp, sfgsymscan_t scanner, const char* fmt, ...);
}

%code requires
{
    #include "sfgsym/config.h"

    typedef void* sfgsymscan_t;
}

%define api.pure full
%define api.push-pull push
%define api.prefix {sfgsym}
%define api.header.include {"sfgsym/parsers/expression_parser.y.h"}
%define api.token.prefix {TOK_}
%define parse.error verbose
%parse-param {sfgsymscan_t scanner}
%locations

%union {
    sfgsym_real literal;
    char* variable;
    struct sfgsym_expr* node;
}

%token EOF 0 "end of file"
%token ERROR 1 "lexer error"

%type <literal> LITERAL
%type <variable> VARIABLE
%type <node> expr
%type <node> list
%type <node> val

%token LITERAL
%token VARIABLE
%token INFINITY

%token '(' ')'
%left '-' '+' '*' '/' ','
%right '^'

%start root

%%
root
  : expr             { sfgsym_expr_parser_give_result(driver, $1); }
  ;
expr
  : expr '+' expr    { $$ = sfgsym_expr_op_create(2, sfgsym_op_add, $1, $3); }
  | expr '-' expr    { $$ = sfgsym_expr_op_create(2, sfgsym_op_sub, $1, $3); }
  | expr '*' expr    { $$ = sfgsym_expr_op_create(2, sfgsym_op_mul, $1, $3); }
  | expr '/' expr    { $$ = sfgsym_expr_op_create(2, sfgsym_op_div, $1, $3); }
  | expr '^' expr    { $$ = sfgsym_expr_op_create(2, sfgsym_op_pow, $1, $3); }
  | '(' expr ')' val { $$ = sfgsym_expr_op_create(2, sfgsym_op_mul, $2, $4); }
  | '(' expr ')'     { $$ = $2; }
  | val '(' list ')' { $$ = sfgsym_expr_op_create(2, sfgsym_op_mul, $1, $3); }
  | val              { $$ = $1; }
  ;
list
  : list ',' expr    { $$ = sfgsym_expr_list_realloc_append($1, $3); }
  | expr             { $$ = sfgsym_expr_list_create(1); sfgsym_expr_list_set($$, 0, $1); }
  ;
val
  : LITERAL          { $$ = sfgsym_expr_literal_create($1); }
  | VARIABLE         { $$ = sfgsym_expr_variable_create($1); }
  | INFINITY         { $$ = sfgsym_expr_infinity_create(); }
  ;
%%

