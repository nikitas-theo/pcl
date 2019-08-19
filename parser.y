%{
#include "header.hpp"
#include <iostream> 
%}

%token T_and "T_and"

%%

var : T_and {printf("VARIABLE\n");}

%%


int main() {
	printf("Started the parsing\n");
	yyparse();
}