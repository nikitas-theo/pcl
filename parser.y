%{
#include "header.hpp"
#include <iostream>
%}

%union {
	char* str;
	double real;
	int num;
	char* op; /* to handle ":=" and other 2 chars operators */

}
/*  syntax: %token <type> token_name  "string"
	"string" and token_name can be used interchangably
*/


%token T_and 		"and"
%token T_array		"array"
%token T_begin		"begin"
%token T_boolean	"boolean"
%token T_char		"char"
%token T_dispose	"dispose"
%token T_div		"div"
%token T_do			"do"
%token T_else		"else"
%token T_end		"end"
%token T_false		"false"
%token T_forward	"forward"
%token T_function 	"function"
%token T_goto 		"goto"
%token T_if 		"if"
%token T_integer	"integer"
%token T_label		"label"
%token T_mod		"mod"
%token T_new		"new"
%token T_nil		"nil"
%token T_not		"not"
%token T_of 		"of"
%token T_or			"or"
%token T_procedure	"procedure"
%token T_program	"program"
%token T_real		"real"
%token T_result		"result"
%token T_return		"return"
%token T_then		"then"
%token T_true		"true"
%token T_var		"var"
%token T_while		"while"

%token T_id 		"id"
%token T_const_int	"integer-const"
%token T_const_real "real-const"
%token T_const_char "char-const"
%token T_string 	"string-literal"

%token T_decl 		":="
%token T_geq 		">="
%token T_leq		"<="
%token T_neq		"<>"

%left '-' '+'
%left '*' '/'

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
	  "integer" | "boolean" | "char"
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
	| l-value ":=" expr  | block | call
	| "if" expr "then" stmt
	| "if" expr "then" stmt "else" stmt
	// Resolve dangling else ?
	| "while" expr "do" stmt
	| "id" ':' stmt | "goto" "id" | "return"
	| "new" l-value | "new" '[' expr ']' l-value
	| "dispose" l-value | "dispose" '[' ']' l-value

/*  this operator need to be definied explicitely
	as it is not a character like '+'.
	Make it 2 operators ':', '=' instead..?
*/

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
	| expr '^' | '(' l-value ')'
	;

r-value:
	  "integer-const" | "true" | "false" | "real-const" | "char-const"
	|  '(' r-value ')' | "nil" | call | '@' l-value
	| unop expr |  expr binop expr
	;

call :
	  "id" '(' ')'
	| "id" '(' expr_list ')'
	;

unop :
	  "not" | '+' | '-'
	;

binop :
	  '+' | '-' | '*' | '/' | "div" | "mod" | "or" | "and"
	| '=' | "<>" | '<' | "<=" | '>' | ">="
	;

%%


int main() {
	printf("Started the parsing\n");
	yyparse();
}
