#ifndef __LEXER_HPP__
#define __LEXER_HPP__
#include <stdio.h>
void yyerror(char const* msg );
extern "C" int yylex();
extern  FILE *yyin;

/*   
     The above command is used to notify g++ to expect a C-like function as yylex().
     The code in lexer.l is compiled with g++ but is a C-source file ( this is the best method for a 
     C++ scanner, as flex++ is problematic). A definition without extern "C" produces errors. 
*/

#endif
