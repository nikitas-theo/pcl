%{
#include "header.hpp"
#include <iostream> 
%}
%define api.value.type union // Do i need this? 
%union {
	... 
	// all possible token types are defined here
}
// syntax: %token <type> token_name  "string"
// "string" and token_name can be used interchangably

%token T_program "program"  
%left ’-’ ’+’
%left ’*’ ’/’

%%

program : 
	  "program" id ';' body '.'
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
	  id ',' id_list 
	| id 
	; 

header : 
	  "procedure" id '(' parameter_list ')'
	| "function" id '(' paremeter_list ')' ':' type
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
	| var id_list ':' type
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
	| l-value ":=" expr | block | call 
	| "if" expr "then" stmt 
	| "if" expr "then" stmt "else" stmt 
	// Resolve dangling else ? 
	| "while" expr "do" stmt 
	| id ':' stmt | "goto" id | "return"
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
	  id | "result" | "string-literal" | l-value '[' expr ']'
	| expr '^' | '(' l-value ')'
	;

r-value: 
	  "integer-const" | "true" | "false" | "real-const" | "char-const"
	|  '(' r-value ')' | "nil" | call | '@' l-value 
	| unop expr |  expr binop expr 
	;

call : 
	  id '(' ')'  
	| id '(' expr_list ')' 
	; 

unop : 
	  "not" | '+' | '-' 
	;

binop : 
	  '+' | '-' | '*' | '/' | "div" | "mod" | "or" | "and" | 
	| '=' | "<>" | '<' | "<=" | '>' | ">=" 
	;
	
%%


int main() {
	printf("Started the parsing\n");
	yyparse();
}