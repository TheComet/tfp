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
%type <node> list
%type <node> expr
%type <node> factor
%type <node> pow
%type <node> impl_mul
%type <node> term
%type <node> val
%type <node> lit
%type <node> var
%type <node> inf

%token LITERAL
%token VARIABLE
%token INFINITY

%token '(' ')'
%token ','
%token '+' '-'
%token '*' '/'
%token '^'

%%
root
  : expr                    { sfgsym_expr_parser_give_result(driver, $1); }
  ;
list
  : expr                    { $$ = sfgsym_expr_list_create(1); sfgsym_expr_list_set($$, 0, $1); }
  | list ',' expr           { $$ = sfgsym_expr_list_realloc_append($$, $3); }
  ;
expr
  : factor                  { $$ = $1; }
  | expr '+' factor         { $$ = sfgsym_expr_op_create(2, sfgsym_op_add, $1, $3); }
  | expr '-' factor         { $$ = sfgsym_expr_op_create(2, sfgsym_op_sub, $1, $3); }
  ;
factor
  : impl_mul                { $$ = $1; }
  | factor '*' impl_mul     { $$ = sfgsym_expr_op_create(2, sfgsym_op_mul, $1, $3); }
  | factor '/' impl_mul     { $$ = sfgsym_expr_op_create(2, sfgsym_op_div, $1, $3); }
  ;
impl_mul
  : pow                     { $$ = $1; }
  | impl_mul pow            { $$ = sfgsym_expr_op_create(2, sfgsym_op_mul, $1, $2); }
  | '-' pow                 { $$ = sfgsym_expr_op_create(2, sfgsym_op_sub, sfgsym_expr_literal_create(0), $2); }
  ;
pow
  : term                    { $$ = $1; }
  | term '^' pow            { $$ = sfgsym_expr_op_create(2, sfgsym_op_pow, $1, $3); }
  | term '^' '-' pow        { $$ = sfgsym_expr_op_create(2, sfgsym_op_pow, $1, sfgsym_expr_op_create(2, sfgsym_op_sub, sfgsym_expr_literal_create(0), $4)); }
  ;
term
  : '(' list ')'            { $$ = $2; }
  | val                     { $$ = $1; }
  ;
val
  : lit                     { $$ = $1; }
  | var                     { $$ = $1; }
  | inf                     { $$ = $1; }
  ;
lit : LITERAL               { $$ = sfgsym_expr_literal_create($1); }
var : VARIABLE              { $$ = sfgsym_expr_variable_create($1); free($1); }
inf : INFINITY              { $$ = sfgsym_expr_infinity_create(); }
%%

