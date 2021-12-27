# A compiler for the PCL language
### Created for the compilers cource (NTUA - 8th semester)


The PCL language is a strict subset of the pascal programming language. Complete definition given (in Greek) in the definition pdf. 

The PCL language requires static scoping where inner functions can access outer functions variables. This is a challenging language feature to implement in llvm IR because of the C-style structure of llvm functions. Many solutions have been proposed in threads etc : 

    * 
    *

In the end I opted for the solution detailed in this thesis by Anh Nguyen [link](https://www.theseus.fi/bitstream/handle/10024/166119/Nguyen_Anh%20.pdf;jsessionid=97A6A61F9CF3E811E7BBB6F4A5E86EAA?sequence=2) 6.2.9 Nested functions. Essentially each function carries a hidden argument which is a struct with any extra variables needed from an outside scope. 


Makefile directives : 
    clean : 
    distclean: 


If you make changes to ast.hpp, execute make distclean before re-compiling, as the makefile is not aware of changes in the ast.hpp code. 


Project structure : 
types.hpp  : type related information 
symbol.hpp/cpp : semantic analysis and the compilation symbol tables 
semantic.cpp : semantic analysis for all AST nodes 
printon.cpp : code used for printing contents of the AST
parser.y : grammar implementation file and the main driving unit of the compiler
libs.c : library functions implemented directly in C, they are then linked with the final llvm code
lexer.l : lexical analysis in Flex 
compile.cpp : all the compilation 
ast.hpp : AST Node definitions 
ast.cpp : required implemenations 

lexer.cpp, parser.hpp/cpp are automatically derived files
lexer.hpp is needed to connect C with C++ utilities
