%option noyywrap
%option nounput

%{
/* C-code here goes verbatin to lex.yy.c */

#include "ast.hpp"  // includes ast.hpp for types 
#include "lexer.hpp" // define yylex(),yyerror(),
#include "parser.hpp" // get token types from Bison

#include <string.h>

static void update_global_location()
{
    yylloc.first_line = yylloc.last_line;
    yylloc.first_column = yylloc.last_column;
    for(int i = 0; yytext[i] != '\0'; i++) 
    {
        if(yytext[i] == '\n')
        {
            yylloc.last_line++;
            yylloc.last_column = 0;
        }
        else {
            yylloc.last_column++;
        }
    }
}

#define YY_USER_ACTION  update_global_location();


%}

L [[:alpha:]]
D [[:digit:]]
E (\\[nrt0"'\\])
C [^"'\\\n]
W [ \t\r\n]

/* C = common chars = printable chars except { ",',\,*,/ } , E = escape, D = digits, L = letters , W = white characters */


%x COMMENT STRING

%%

%{
    /* C code here is local to part 2 and 3,
    any code here is executed everytime yylex() is called */

%}

"and"       { yylval.op = strdup(yytext); return T_and; }
"div"       { yylval.op = strdup(yytext);  return T_div; }
"mod"       { yylval.op = strdup(yytext); return T_mod; }
"not"       { yylval.op = strdup(yytext); return T_not; }
"or"        { yylval.op = strdup(yytext); return T_or; }

"array"     { return T_array; }
"begin"     { return T_begin; }
"boolean"   { return T_boolean; }
"char"      { return T_char; }
"dispose"   { return T_dispose; }
"do"        { return T_do; }
"else"      { return T_else; }
"end"       { return T_end; }
"false"     { return T_false; }
"forward"   { return T_forward; }
"function"  { return T_function; }
"goto"      { return T_goto; }
"if"        { return T_if; }
"integer"   { return T_integer; }
"label"     { return T_label; }
"new"       { return T_new; }
"nil"       { return T_nil; }
"of"        { return T_of; }
"procedure" { return T_procedure; }
"program"   { return T_program; }
"real"      { return T_real; }
"result"    { return T_result; }
"return"    { return T_return; }
"then"      { return T_then; }
"true"      { return T_true; }
"var"       { return T_var; }
"while"     { return T_while; }

{L}([[:alnum:]]|_)*         { yylval.str = strdup(yytext); return T_id;		/* Identifiers */ }
{D}+                        { yylval.num = atoi(yytext); return T_const_int;	/* Integers */ }
{D}+\.{D}+([eE][+-]?{D}+)?  { yylval.real = atof(yytext); return T_const_real;	/* Reals */ }
\'({C}|{E})\'               { yylval.ch = yytext[0]; return T_const_char;	/* Chars */ }


[+\-*//^@><=]   |   /* same as the action of next rule */
[;.(),\[\]:=]   { yylval.op = strdup(yytext); return yytext[0]; }
":="            { yylval.op = strdup(yytext); return T_assign; }
"<>"            { yylval.op = strdup(yytext); return T_neq; }
">="            { yylval.op = strdup(yytext); return T_geq; }
"<="            { yylval.op = strdup(yytext); return T_neq; }

\"                      { BEGIN(STRING); }
<STRING>({C}|{E})+      { yylval.str = strdup(yytext); }
<STRING>\"              { BEGIN(INITIAL); return T_string; }
<STRING>\n              { yyerror("Unfinished string literal"); }
<STRING><<EOF>>         { yyerror("Unfinished string literal"); yyterminate(); }


"(*"                { BEGIN(COMMENT); }
<COMMENT>[^*]*      { /* eat up anything that is not a '*' */ }
<COMMENT>"*)"       { BEGIN(INITIAL); }
<COMMENT>"*"        { /* eat up *'s not followed by ')' */ }
<COMMENT><<EOF>>    { yyerror("Unfinished comment - expected *)"); yyterminate(); }


{W}+        {  /* do nothing */ }
<*>.        { yyerror("lexical error"); }

%%

void yyerror (const char *msg) {
    fprintf (stderr, "At %d:%d : %s\n",yylloc.first_line, yylloc.first_column, msg);
}
