%{
#include "lexer.hpp"
#include "ast.hpp"
#include "symbol.hpp"

std::string program_name; 
bool imm_stdout; 
bool optimize; 
bool print_ast; 
%}

%union {
    int num;
    double real;
    char *str;
    char *op;
    char ch;
    Stype type;
    Expr* expr;
    Stmt* stmt;
    Block* prog;
    VarDef* vgs;
    FunctionDef* rtn;
    /* FormalsGroup* flg; */
    ParameterGroup* flg;
    /* std::vector<std::string>* ids; */
    std::list<std::string>* ids;
    /* ASTvector<FormalsGroup*>* fgs; */
    std::list<ParameterGroup*>* fgs;
    /* ASTvector<Stmt*>* sts; */
    ASTnodeCollection* sts;
    /* ASTvector<Expr*>*  exs; */
    ASTnodeCollection* exs;
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

%token<str> T_id    "id"

%token<num> T_const_int     "integer-const"
%token<real> T_const_real   "real-const"
%token<ch> T_const_char    "char-const"
%token<str> T_string        "string-literal"
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

%type<op> binop_high binop_med binop_low 
%type<op> sign
%type<type> type
%type<expr> expr r_value l_value ll_value func_call
%type<stmt> local stmt proc_call
%type<prog> body
%type<vgs> var_def
%type<ids> id_list
%type<rtn> header
%type<flg> formal
%type<fgs> parameter_list formal_list
%type<sts> block stmt_list
%type<exs> expr_list
%%

program :
      "program" T_id ';' body '.' {
        std::cout << "Parsed" << std::endl;
        $4->semantic_analysis(); 
        if (print_ast) {
          std::cout << "AST:\n" << *$4 << std::endl; 
        }
        if (program_name == "") program_name = $2;
        //$4->compile_llvm(program_name,optimize, imm_stdout); 
      }
    ;

body:
      local body            { $2->push_local($1) ; $$ = $2; }
    | block                 { $$ = new Block($1, yylloc.last_line); }
    ;

local:
      "var" var_def         { $$ = $2; }
    | "label" id_list ';'   { $$ = new LabelDef($2, yylloc.last_line); }
    | header ';' body ';'   { $$ = $1; $1->add_body($3); }
    | "forward" header ';'  { $$ = $2; $2->set_forward(); }
    ;

var_def:
      id_list ':' type ';' var_def  { $5->push($1, $3) ; $$ = $5; }
    | id_list ':' type ';'          { $$ = new VarDef(yylloc.last_line); $$->push($1, $3); }
    ;

id_list :
      T_id ',' id_list              { $3->push_front($1) ; $$ = $3; }
    | T_id                          { $$ = new std::list<std::string>(); $$->push_front($1); }
    ;

header :
	  "procedure" T_id '(' parameter_list ')'           { $$ = new FunctionDef($2, $4, typeVoid, yylloc.last_line); }
	| "function" T_id '(' parameter_list ')' ':' type   { $$ = new FunctionDef($2, $4, $7, yylloc.last_line); }
	;

parameter_list: 
      %empty                        { $$ = new std::list<ParameterGroup*>(); } /* If we have no params then we need an empty stack */
    | formal formal_list            { $2->push_front($1); $$ = $2; }  /* Even if formal_list is empty, we should have a stack from the next rule */
    ;

formal_list: 
      %empty                        { $$ = new std::list<ParameterGroup*>(); }
    | ';' formal formal_list        { $3->push_front($2) ; $$ = $3; } 
    ;

formal:
      id_list ':' type              { $$ = new ParameterGroup {*$1, $3, PASS_BY_VALUE}; /* $$ = new FormalsGroup($1, $3, PASS_BY_VALUE); */ }
    | "var" id_list ':' type        { $$ = new ParameterGroup {*$2, $4, PASS_BY_REFERENCE}; /* $$ = new FormalsGroup($2, $4, PASS_BY_REFERENCE); */ }
    ;

type :
      "integer"                     { $$ = typeInteger;}
    | "real"                        { $$ = typeReal;}
    | "boolean"                     { $$ = typeBoolean;}
    | "char"                        { $$ = typeChar;}
    | "array" "of" type[refType]    { $$ = typeIArray($refType); }
    | "array" '[' "integer-const"[size] ']' "of" type[refType] 	{ $$ = typeArray($size, $refType); }
    | '^' type                      { $$ =  typePointer($2) ; }
    ;

block:
      "begin" stmt_list "end"       { $$ = $2; }
    ;

stmt_list :
      stmt                          { $$ = new ASTnodeCollection(); $$->push($1); }
    | stmt ';' stmt_list            { $3->push($1) ; $$ = $3; }
    ;

stmt:
      %empty                        { $$ = new EmptyStmt(yylloc.last_line);}
    | l_value T_decl expr           { $$ = new Declaration($1, $3, yylloc.last_line); } 
    | block                         { $$ = $1; }
    | proc_call                     { $$ = $1; }
    | "if" expr "then" stmt                 { $$ = new IfThenElse($2, $4,yylloc.last_line); }
    | "if" expr "then" stmt "else" stmt     { $$ = new IfThenElse($2, $4, $6, yylloc.last_line); } 
    | "while" expr "do" stmt        { $$ = new While($2, $4, yylloc.last_line); }
    | T_id ':' stmt                 { $$ = new Label($1, $3, yylloc.last_line); }
    | "goto" T_id                   { $$ = new GoTo($2, yylloc.last_line); }
    | "return"                      { $$ = new ReturnStmt(yylloc.last_line); }
    | "new" l_value                 { $$ = new Init($2, yylloc.last_line); } 
    | "new" '[' expr ']' l_value    { $$ = new InitArray($5, $3, yylloc.last_line); }
    | "dispose" l_value             { $$ = new Dispose($2, yylloc.last_line); } 
    | "dispose" '[' ']' l_value     { $$ = new DisposeArray($4, yylloc.last_line); }
    ;

expr:
      l_value           { $$ = $1; }
    | r_value           { $$ = $1; }
    ;

expr_list:
      expr                      { $$ = new ASTnodeCollection(); $$->push($1); }
    | expr ',' expr_list        { $3->push($1); $$ = $3; }
    ;


r_value:
      "integer-const"               { $$ = new Const($1,typeInteger, yylloc.last_line); } 
    | "real-const"                  { $$ = new Const($1,typeReal, yylloc.last_line); }
    | "char-const"                  { $$ = new Const($1,typeChar, yylloc.last_line); } 
    | "true"                        { $$ = new Const(true,typeBoolean, yylloc.last_line); }
    | "false"                       { $$ = new Const(false,typeBoolean, yylloc.last_line); }
    | '(' r_value ')'               { $$ = $2; }
    | "nil"                         { $$ = new Const(0,typeVoid, yylloc.last_line); } 
    | func_call                     { $$ = $1; }
    | '@' ll_value                  { $$ = new UnOp($1, $2, yylloc.last_line); }
    | "not" expr                    { $$ = new UnOp($1, $2, yylloc.last_line); }
    | sign expr %prec U_SIGN        { $$ = new UnOp($1, $2, yylloc.last_line); }
    | expr binop_high expr %prec '*'    { $$ = new BinOp($1, $2, $3, yylloc.last_line); }
    | expr binop_med expr %prec '+'     { $$ = new BinOp($1, $2, $3, yylloc.last_line); }
    | expr binop_low expr %prec '='     { $$ = new BinOp($1, $2, $3, yylloc.last_line); }



l_value:
      T_id                      { $$ = new Id($1, yylloc.last_line); }
    | "result"                  { $$ = new Result(yylloc.last_line); }
    | "string-literal"          { $$ = new StringValue($1, yylloc.last_line); }
    | l_value '[' expr ']'      { $$ = new ArrayAccess($1, $3, yylloc.last_line); }
    | expr '^'                  { $$ = new Dereference($1, yylloc.last_line); }
    | '(' l_value ')'           { $$ = $2; }
    ;

ll_value:
      T_id                      { $$ = new Id($1, yylloc.last_line); }
    | "result"                  { $$ = new Result(yylloc.last_line); }
    | "string-literal"          { $$ = new StringValue($1, yylloc.last_line);  }
    | ll_value '[' expr ']'     { $$ = new ArrayAccess($1, $3, yylloc.last_line); } 
    | '(' l_value ')'           { $$ = $2; }
    ;

func_call :
      T_id '(' ')'              { $$ = new CallFunc($1, yylloc.last_line); }
    | T_id '(' expr_list ')'    { $$ = new CallFunc($1, $3, yylloc.last_line); }
    ;

proc_call :
      T_id '(' ')'              {$$ =  new CallProc($1, yylloc.last_line);}
    | T_id '(' expr_list ')'    {$$ =  new CallProc($1, $3,yylloc.last_line);}
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
  optimize = false; 
  imm_stdout = false; 
  print_ast = false; 
  program_name = "";
  for (int i = 0 ; i < argc ; i++){
    std::string arg(argv[i]);
    if (arg.find("-O") != std::string::npos) optimize = true; 
    if (arg.find("-i") != std::string::npos) imm_stdout = true;   
    if (arg.find("--ast") != std::string::npos) print_ast = true; 

  }




  // read from cmd argument file
  if (! imm_stdout){

    std::string file(argv[1]);
    std::string base_filename = file.substr(file.find_last_of("/\\") + 1);
    std::string::size_type const p(base_filename.find_last_of('.'));
    std::string file_without_extension = base_filename.substr(0, p);

    program_name = file_without_extension;

    yyin = fopen(argv[1], "r");
  
  }


  // read from stdin
  int ret = yyparse();
  fclose(yyin);


}
