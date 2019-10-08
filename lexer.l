%{
/* C-code here goes verbatin to lex.yy.c */

#include "parser.hpp" // get token types from Bison
#include "header.hpp" // define yylex(),yyerror()
#include <string.h>

#define YY_USER_ACTION ; // will be used for location
%}

L [[:alpha:]]
<<<<<<< HEAD
D [0-9] 
E (\\[ntr0\\'"])
C [^"'\\\n]
W [ \t\r\n]
/* C = common chars, E = escape, D = digits, L = letters */
/* warning: don't use nested character classes. [L] doesn't work. */
/* TODO: fill out more keywords*/
=======
D [[:digit:]]
E (\\[nrt0"'\\])
C [^"'\\\n]
W [ \t\r\n]

/* C = common chars = printable chars except { ",',\,*,/ } , E = escape, D = digits, L = letters , W = white characters */
>>>>>>> ac446e6... Further developed lexer, parser


%x comment str

%%

%{
    /* C code here is local to part 2 and 3,
    any code here is executed everytime yylex() is called */

%}
<<<<<<< HEAD
 
"and" {printf("T_and\n"); return(T_and);}
"array" {printf("T_array\n");}
"begin" {printf("T_begin\n");}
"boolean" {printf("T_boolean\n");}

{L}({L}|{D}|_)* printf("Identifier - Lexeme is: %s\n",yytext);
{D}+  printf("Integer - Lexeme is: %s\n",yytext);
{D}+"."{D}+([eE][+-]?{D}*)? printf("Real num - Lexeme is: %s\n",yytext);
\'({C}|{E})\' printf("char - Lexeme is: %s\n",yytext);
([+\-*//^@]|<(>|=)?|>=?) printf("operant lexeme: %s\n",yytext);
[;.(),\[\]]|(:=?) printf("delimiter lexeme: %s\n",yytext);

\" BEGIN(str);


<str>{
    ({C}|{E})+ { 
        
        /* Empty strings are not allowed. */
        printf("string - Lexeme is: %s\n",yytext);
    }
    \" {
        /* in this rule return token, and string in yyval */
        BEGIN(INITIAL);
    }
    \n {
        /* in this one handle error, also count newline? */
        /* TODO: should use yyerror(); */
        printf("Error - unfinished string literal\n");
        BEGIN(INITIAL); /* just for testing, remember to delete this and actually handle */
    }
    <<EOF>> {
        printf("Error - unfinished string literal\n");
        yyterminate();
    }
    
}

{W} /* do nothing, should I count lines for error messages? */

"(*" {
    BEGIN(comment);
    printf("reading comment\n");
}

<comment>{
    
    [^*\n]* /* eat up anything that is not a '*' */
    "*"+[^*\)\n]* /* eat up *'s not followed by ')' */ 
    \n /* count newlines?*/
    "*"+")" {
    /* the + op is needed, or 2nd rule will eat up our comment ending */
                printf("ending comment\n");
                BEGIN(INITIAL); 
    /* comment rules taken from lexer manual pg. 31, they are designed to eat as much input as possible for a faster scanner */ 
    }
    <<EOF>> {
    printf("Error - unfinished comment\n");
    yyterminate();
    }
}

<INITIAL,str>. {
    /* default rule for error */
    /* TODO: handle with yyerror() */
    printf("Error - unmatched input\n");
    }
%%
void yyerror (char const *s)
{
    fprintf (stderr, "%s\n", s);
}
=======

"and"       { return T_and; }
"array"     { return T_array; }
"begin"     { return T_begin; }
"boolean"   { return T_boolean; }
"char"      { return T_char; }
"dispose"   { return T_dispose; }
"div"       { return T_div; }
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
"mod"       { return T_mod; }
"new"       { return T_new; }
"nil"       { return T_nil; }
"not"       { return T_not; }
"of"        { return T_of; }
"or"        { return T_or; }
"procedure" { return T_procedure; }
"program"   { return T_program; }
"real"      { return T_real; }
"result"    { return T_result; }
"return"    { return T_return; }
"then"      { return T_then; }
"true"      { return T_true; }
"var"       { return T_var; }
"while"     { return T_while; }

{L}([[:alnum:]]|_)*     	{ yylval.str = strdup(yytext); return T_id; }			/* Identifiers */
{D}+                		{ yylval.num = atoi(yytext); return T_const_int; } 	/* Integers */
{D}+\.{D}+([eE][+-]?{D}+)?  { yylval.real = atof(yytext); return T_const_real; } /* Reals */
\'({C}|{E})\'           	{ yylval.op = strdup(yytext); return T_const_char; }	/* chars */


[+\-*//^@><=] 	|   /* same as the action of next rule */
[;.(),\[\]:=] 			{
								yylval.op = strdup(yytext);
								return yytext[0];
							}
":="	{ yylval.op = strdup(yytext);	return T_decl; }
"<>"	{ yylval.op = strdup(yytext);	return T_neq; }
">="	{ yylval.op = strdup(yytext);	return T_geq; }
"<="	{ yylval.op = strdup(yytext);	return T_neq; }

\"                      { BEGIN(STRING); }
<STRING>({C}|{E})+ 		{
							yylval.str = strdup(yytext);
							/* Moved it first for rule priority (?) */
						}
<STRING>\"              { BEGIN(INITIAL); return T_string; }
<STRING>\n              { yyerror("Unfinished string literal"); }
<STRING><<EOF>>			{ yyerror("Unfinished string literal"); yyterminate();}


<COMMENT>[^*\n]* 		{ /* eat up anything that is not a '*' */ }
<COMMENT>"*"+[^*\)\n]*  { /* eat up *'s not followed by ')' */ }
<COMMENT>"*"+")" 		{
  							/* the + op is needed, or 2nd rule will eat up our comment ending */
                			BEGIN(INITIAL);
					    }

<COMMENT><<EOF>> 		{
    						yyerror("Unfinished comment - expected *)");
    						yyterminate();
    					}




{W} 		{ /* do nothing */ }
<*>.        { yyerror("lexical error"); }

%%

void yyerror (const char *msg) {
    fprintf (stderr, "%s\n", msg);
}
>>>>>>> ac446e6... Further developed lexer, parser
