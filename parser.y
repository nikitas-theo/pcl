%{
#include "header.hpp"
#include <iostream>


%}

%union {
	Expr* expr;
	char* str;
	double real;
	int num;
	char c; 
	char* op;

}
%locations
%define parse.lac full
%define parse.error verbose
/*  syntax: %token <type> token_name  "string"
	"string" and token_name can be used interchangably

	%left, %right, %precedence, or %nonassoc : to specify precedence
*/


%token T_array		"array"
%token T_begin		"begin"
%token T_boolean	"boolean"
%token T_char			"char"
%token T_dispose	"dispose"
%token T_do				"do"

%token T_end			"end"
%token T_false		"false"
%token T_forward	"forward"
%token T_function "function"
%token T_goto 		"goto"
%token T_if 			"if"
%token T_integer	"integer"
%token T_label		"label"

%token T_new			"new"
%token T_nil			"nil"

%token T_of 			"of"
%token T_procedure "procedure"
%token T_program	"program"
%token T_real			"real"
%token T_result		"result"
%token T_return		"return"

%token T_true			"true"
%token T_var			"var"
%token T_while		"while"

%token T_id 			"id"
%token T_const_int "integer-const"
%token T_const_real "real-const"
%token T_const_char "char-const"
%token T_string 	"string-literal"

%precedence T_then
%precedence T_else

%left T_decl T_geq T_leq T_neq '<' '>' '='
%left '-' '+' T_or
%left T_div T_mod T_and '*' '/'
%left T_not
%left USIGN
%precedence '^'
%precedence '@'
%precedence '[' ']'

%token END 0 "end of file" /* nice error messages */


%%

program :
	  "program" "id" ';' body '.'
	;

body :
	  local body
	| block
	;

local :
	  "var" var_def
	| "label" id_list ';'
	| header ';' body ';'
	| "forward" header ';'
	;

var_def :
	  id_list ':' type ';' var_def
	| id_list ':' type ';'
	;

id_list :
	  "id" ',' id_list
	| "id"
	;

header :
	  "procedure" "id" '(' parameter_list ')'
	| "function" "id" '(' parameter_list ')' ':' type
	;

parameter_list :
	  %empty
	| formal formal_list
	;

formal_list :
	  %empty
	| ';' formal
	;

formal :
	  id_list ':' type
	| "var" id_list ':' type
	;

type :
	  "integer" | "real" | "boolean" | "char"
	| "array" "of" type
	| "array" '[' "integer-const" ']' "of" type
	| '^' type
	;

block :
	  "begin" stmt_list "end"
	;
stmt_list :
	  stmt
	| stmt ';' stmt_list
	;

stmt :
	  %empty
	| l-value T_decl expr  | block | call
	| "if" expr T_then stmt
	| "if" expr T_then stmt T_else stmt

	| "while" expr "do" stmt
	| "id" ':' stmt | "goto" "id" | "return"
	| "new" l-value | "new" '[' expr ']' l-value
	| "dispose" l-value | "dispose" '[' ']' l-value
	;

expr :
	  l-value
	| r-value
	;


expr_list:
	  expr
	| expr ',' expr
	;

l-value:
	  "id" | "result" | "string-literal" | l-value '[' expr ']'
	 | '(' l-value ')' | expr '^'
	;


r-value:
	  "integer-const" | "true" | "false" | "real-const" | "char-const"
	| '(' r-value ')' | "nil" | call | '@' ll-value
	| T_not expr |  sign expr %prec USIGN |  expr binop_1 expr %prec '='| expr binop_2 expr %prec '+'| expr binop_3 expr %prec '*';
	;

	ll-value:
		  "id" | "result" | "string-literal" | ll-value '[' expr ']'
		 | '(' l-value ')'

call :
	  "id" '(' ')'
	| "id" '(' expr_list ')'
	;

sign :
	  '+'   | '-'
	;

binop_1 : 	  '=' | T_neq | '<' | T_leq | '>' | T_geq ;
binop_2 : 	  '+' | '-' | T_or ;
binop_3 : 	  '*' | '/' | T_div | T_mod | T_and ;




%%


int main() {
	#if YYDEBUG
		yydebug = 1;
	#endif

	if(!yyparse())
	printf("Parse successful.\n");
}
