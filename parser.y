%{

#include "header.hpp"
#include "ast.hpp"

%}

%union {
	Expr* expr;
	char* str;
	double real;
	int num;
	char* op;

}

%locations

%define parse.lac full
%define parse.error verbose

%token T_and		"and"
%token T_array		"array"
%token T_begin		"begin"
%token T_boolean	"boolean"
%token T_char		"char"
%token T_dispose	"dispose"
%token T_div		"div"
%token T_do		"do"
%token T_else		"else"
%token T_end		"end"
%token T_false		"false"
%token T_forward	"forward"
%token T_function	"function"
%token T_goto		"goto"
%token T_if		"if"
%token T_integer	"integer"
%token T_label		"label"
%token T_mod		"mod"
%token T_new		"new"
%token T_nil		"nil"
%token T_not		"not"
%token T_of		"of"
%token T_or		"or"
%token T_procedure	"procedure"
%token T_program	"program"
%token T_real		"real"
%token T_result		"result"
%token T_return		"return"
%token T_then		"then"
%token T_true		"true"
%token T_var		"var"
%token T_while		"while"

%token T_id		"id"

%token T_const_int 	"integer-const"
%token T_const_real 	"real-const"
%token T_const_char 	"char-const"
%token T_string 	"string-literal"

%precedence T_then
%precedence T_else

%left T_decl T_geq T_leq T_neq '<' '>' '='
%left '-' '+' T_or
%left T_div T_mod T_and '*' '/'
%left T_not
%left U_SIGN

%precedence '^'
%precedence '@'
%precedence '[' ']'

%token END 0 "end of file" /* nice error messages */

%type <expr> r_value
%type <expr> expr
%type <str> binop_low

%%

program :
	  "program" T_id ';' body '.'
	;

body:
	  local body
	| block
	;

local:
	  "var" var_def
	| "label" id_list ';'
	| header ';' body ';'
	| "forward" header ';'
	;

var_def:
	  id_list ':' type ';' var_def
	| id_list ':' type ';'
	;

id_list :
	  T_id ',' id_list
	| T_id
	;

header :
	  "procedure" T_id '(' parameter_list ')'
	| "function" T_id '(' parameter_list ')' ':' type
	;

parameter_list:
	  %empty
	| formal formal_list
	;

formal_list:
	  %empty
	| ';' formal
	;

formal:
	  id_list ':' type
	| "var" id_list ':' type
	;

type :
	  "integer" 
	| "real" 
	| "boolean" 
	| "char"
	| "array" "of" type
	| "array" '[' "integer-const" ']' "of" type
	| '^' type
	;

block:
	  "begin" stmt_list "end"
	;

stmt_list :
	  stmt
	| stmt ';' stmt_list
	;

stmt:
	  %empty
	| l_value T_decl expr  
	| block 
	| call
	| "if" expr "then" stmt
	| "if" expr "then" stmt "else" stmt
	| "while" expr "do" stmt
	| T_id ':' stmt 
	| "goto" T_id 
	| "return"
	| "new" l_value 
	| "new" '[' expr ']' l_value
	| "dispose" l_value 
	| "dispose" '[' ']' l_value
	;

expr:
	  l_value 
	| r_value
	;

expr_list:
	  expr
	| expr ',' expr_list
	;

l_value:
	  T_id 
	| "result" 
	| "string-literal" 
	| l_value '[' expr ']'
	| expr '^'
	| '(' l_value ')'
	;


r_value:
	  "integer-const" 
	| "real-const" 
	| "char-const"
	| "true" 
	| "false" 
	| '(' r_value ')' 
	| "nil" 
	| call 
	| '@' ll_value
	| "not" expr 
	| sign expr %prec U_SIGN 
	| expr binop_high expr %prec '*'
	| expr binop_med expr %prec '+'
	| expr binop_low expr %prec '=' { $$ = new Op($1, $2, $3); }
	;

ll_value:
	  T_id 
	| "result" 
	| "string-literal" 
	| ll_value '[' expr ']'
	| '(' l_value ')'
	;

call :
	  T_id '(' ')'
	| T_id '(' expr_list ')'
	;

sign :
	  '+' | '-'
	;

binop_low: 
	  '=' | T_neq | '<' | T_leq | '>' | T_geq
	;

binop_med:
	  '+' | '-' | T_or
	;

binop_high:
	  '*' | '/' | T_div | T_mod | T_and
	;


%%


int main() {

	#if YYDEBUG
		yydebug = 1;
	#endif
	if(!yyparse())
	std::cout << "Parse successful.\n";
}
