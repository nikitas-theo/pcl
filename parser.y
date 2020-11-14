%{

#include "header.hpp"
#include "ast.hpp"
#include "symbol/symbol.h"
%}

%union {
    Expr* expr;
    Stmt* stmt;
    Body* body;
    VarDef* vardef;
    ExprList* expr_list;
    StmtList* stmt_list; 
    char* str;
    double real;
    int num;
    char* op;
    Type type;

}

%locations

%define parse.lac full
%define parse.error verbose

%token T_and        "and"
%token T_array      "array"
%token T_begin      "begin"
%token T_boolean    "boolean"
%token T_char       "char"
%token T_dispose    "dispose"
%token T_div        "div"
%token T_do         "do"
%token T_else       "else"
%token T_end        "end"
%token T_false      "false"
%token T_forward    "forward"
%token T_function   "function"
%token T_goto       "goto"
%token T_if         "if"
%token T_integer    "integer"
%token T_label      "label"
%token T_mod        "mod"
%token T_new        "new"
%token T_nil        "nil"
%token T_not        "not"
%token T_of         "of"
%token T_or         "or"
%token T_procedure  "procedure"
%token T_program    "program"
%token T_real       "real"
%token T_result     "result"
%token T_return     "return"
%token T_then       "then"
%token T_true       "true"
%token T_var        "var"
%token T_while      "while"

%token<str> T_id         "id"

%token<num> T_const_int      "integer-const"
%token<real> T_const_real     "real-const"
%token<str> T_const_char     "char-const"
%token<str> T_string         "string-literal"

%precedence T_then
%precedence T_else

%left<op> T_decl T_geq T_leq T_neq '<' '>' '='
%left<op> '-' '+' T_or
%left<op> T_div T_mod T_and '*' '/'
%left<op> T_not
%left<op> U_SIGN

%precedence<op> '^'
%precedence<op> '@'
%precedence '[' ']'

%token END 0 "end of file" /* nice error messages */

%type<expr>  l_value ll_value r_value  expr 
%type<expr_list> expr_list

%type<expr> func_call
%type<stmt> proc_call
%type<body> body
%type<vardef> var_def
%type<stmt> stmt  header block formal 
%type<stmt> program  local   
%type<stmt_list> stmt_list formal_list parameter_list id_list 
%type<type> type 
%type<op> binop_high binop_med binop_low sign 

%%

program :
      "program" T_id ';' body '.' {cout << "AST: " << *$4 << endl;}
    ;

body:
      local body {$2->append($1) ; $$ = $2;}
    | block {$$ = new Body($1);}
    ;

local:
      "var" var_def {$$ = new Local(var_def,$2);}
    | "label" id_list ';' {$$ = new Local(label_def,$2);}
    | header ';' body ';'{$$ = new Local(func_def,$$);}
    | "forward" header ';' {$$ = new Local(forward_def,$2);}
    ;

var_def:
      id_list[id] ':' type[t] ';' var_def[defs] {$defs->append($id,$t) ; $$ = $defs;}
    | id_list ':' type ';' {$$ = new VarDef($1,$3);}
    ;

id_list :
      T_id ',' id_list {$3->append( new Id($1)) ; $$ = $3;}
    | T_id {$$ = new StmtList( new Id($1)); }
    ;

header :
      "procedure" T_id[name] '(' parameter_list[params] ')' {$$ = new Header(procedure, $name, $params);}
    | "function" T_id[name] '(' parameter_list[params] ')' ':' type[t] {$$ = new Header(function, $name, $params,$t);}
    ;

parameter_list: 
      %empty  {$$ = new StmtList();}
    | formal formal_list {$2->append($1); $$ = $2;}
    ;

formal_list: 
      %empty {$$ = new StmtList();}
    | ';' formal formal_list {$3->append($2) ; $$ = $3;} 
    ;

formal:
      id_list ':' type {$$ = new Formal(byvalue, $1,$3);}
    | "var" id_list ':' type  {$$ = new Formal(byreference,$2,$4 );}
    ;

type :
      "integer" {$$ = new Type_tag({TYPE_INTEGER}); }
    | "real" {$$ = new Type_tag({TYPE_REAL}); }
    | "boolean" {$$ = new Type_tag({TYPE_BOOLEAN}); }
    | "char" {$$ = new Type_tag({TYPE_CHAR});}
    | "array" "of" type {$$ = new Type_tag({TYPE_IARRAY,$3}); }
    | "array" '[' "integer-const"[size] ']' "of" type[refType] {$$ = new Type_tag({TYPE_ARRAY,$refType,$size}); }
    | '^' type {$$ =  new Type_tag({TYPE_POINTER,$2}) ; }
    ;

block:
      "begin" stmt_list "end"  {$$ = $2;}
    ;

stmt_list :
      stmt {$$ = new StmtList($1); }
    | stmt ';' stmt_list {$3->append($1) ; $$ = $3;}
    ;

stmt:
      %empty {$$  = new Empty();}
    | l_value T_decl expr { $$ = new Decl($1,$3);} 
    | block 
    | proc_call 
    | "if" expr[cond] "then" stmt[then] {$$ = new IfThenElse($cond,$then); }
    | "if" expr[cond] "then" stmt[then] "else" stmt[else] {$$ = new IfThenElse($cond,$then,$else);} 
    | "while" expr "do" stmt { $$ = new While($2,$4);}
    | T_id ':' stmt  {$$ = new Label($1,$3);}
    | "goto" T_id {$$ = new GoTo($2);}
    | "return" {$$ = new ReturnStmt();}
    | "new" l_value {$$ = new Init($2);} 
    | "new" '[' expr[size] ']' l_value[lval] {$$ = new Init($lval,$size);}
    | "dispose" l_value {$$ = new Dispose($2);} 
    | "dispose" '[' ']' l_value {$$ = new Dispose($4);}
    ;

expr:
      l_value 
    | r_value 
    ;

expr_list:
      expr {$$ = new ExprList($1);}
    | expr ',' expr_list  {$3->append($1) ; $$ = $3;}
    ;


r_value:
      "integer-const" {$$ = new Const<int>($1);} 
    | "real-const" {$$ = new Const<double>($1);}
    | "char-const" {$$ = new Const<char*>($1);} 
    | "true"  {$$ = new Const<bool>(true);}
    | "false" {$$ = new Const<bool>(false);}
    | '(' r_value ')' {$$ = $2;}
    | "nil" {$$ = new Const<NIL>(0);}
    | func_call {$$ = $1;}
    | '@' ll_value	{$$ = new UniOp($1,$2);}
    | "not" expr {$$ = new UniOp($1,$2);}
    | sign expr %prec U_SIGN {$$ = new UniOp($1,$2);}
    | expr binop_high expr %prec '*' {$$ = new BinOp($1,$2,$3);}
    | expr binop_med expr %prec '+' {$$ = new BinOp($1,$2,$3);}
    | expr binop_low expr %prec '=' {$$ = new BinOp($1,$2,$3);}


l_value:
      T_id  {$$ = new Id($1);}
    | "result" {$$ = new Id("result");} // put in the same class
    | "string-literal" {$$ = new String($1);} 
    | l_value '[' expr ']' {$$ = new Access($1,$3);}
    | expr '^' {$$ = new Dereference($1);}
    | '(' l_value ')' {$$ = $2;}
    ;

ll_value:
      T_id {$$ = new Id($1);}
    | "result" {$$ = new Id("result");}
    | "string-literal" {$$ = new String($1);} 
    | ll_value '[' expr ']' {$$ = new Access($1,$3);} 
    | '(' l_value ')' {$$ = $2;}
    ;

func_call :
      T_id '(' ')' {$$ = new Call($1);}
    | T_id '(' expr_list ')' {$$ = new Call($1,$3);}
    ;

proc_call :
      T_id '(' ')' {$$ =  new Call($1);}
    | T_id '(' expr_list ')' {$$ =  new Call($1,$3);}
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


int main(int argc, char *argv[]) {
  #if YYDEBUG
      yydebug = 1;
  #endif
  if (argc == 2) {
    yyin = fopen(argv[1], "r");
  }
  int ret = yyparse();
  if (!ret) { cout << "Parse successful.\n";}

  fclose(yyin);

}
