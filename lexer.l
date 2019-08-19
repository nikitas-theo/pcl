%{
/* C-code here goes verbatin to lex.yy.c */
#include "parser.hpp" // get token types from Bison
#include "header.hpp" // define yylex(),yyerror()
%}

L [[:alpha:]]
D [0-9] 
E (\\[ntr0\\'"])
C [^"'\\\n]
W [ \t\r\n]
/* C = common chars, E = escape, D = digits, L = letters */
/* warning: don't use nested character classes. [L] doesn't work. */
/* TODO: fill out more keywords*/


%x comment str

%%

%{ 
    /* C code here is local to part 2 and 3, 
    any code here is executed everytime yylex() is called */
%}
 
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