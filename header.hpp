#ifndef __HEADER_HPP__
#define __HEADER_HPP__

void yyerror(char const* msg );
extern "C" int yylex();

/*   
     The above command is used to notify g++ to expect a C-like function as yylex().
     The code in lexer.l is compiled with g++ but is a C-source file ( this is the best method for a 
     C++ scanner, as flex++ is problematic). A definition without extern "C" produces errors. 
*/

#endif
