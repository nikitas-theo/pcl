# A compiler for the PCL language
### Created for the compilers cource (NTUA - 8th semester)


The PCL language is a strict subset of the pascal programming language, enriched by small extra features. Complete definition given (in Greek) in the definition pdf. Some basic features of the language : 

* Syntax similar to Pascal
* Scoping as Pascal
* Integer, Boolean, Real and Char primitive types
* Arrays with fixed or unknown size and dynamic memmory allocation
* Standard library functions 


The PCL language requires static scoping where inner functions can access outer functions variables. This is a challenging language feature to implement in llvm IR because of the C-style structure of llvm functions. 

Different solutions where discussed and one can find similar problems in online forums. In the end we opted for the solution detailed in [this](https://www.theseus.fi/bitstream/handle/10024/166119/Nguyen_Anh%20.pdf;jsessionid=97A6A61F9CF3E811E7BBB6F4A5E86EAA?sequence=2) text by Anh Nguyen paragraph 6.2.9 - Nested functions. Essentially each function carries a hidden argument which is a struct with any extra variables needed from an outside scope.

Although the language supports manual dynamic memory allocation and freeing, we have included the [Boehm](https://hboehm.info/gc/) garbage collector. 

-------------------
## Usage
If no `-i` flag is specified then running ./pcl /path/to/codefile.pcl will compile the program and output codefile.imm (the llvm code) and codefile.out the executable in the /path/to/ directory. 

The `-i` flag will read the program from stdin and output llvm code to stdout. 

The `-O` flag will use the llvm optimization modules to optimize the IR.

The `--ast` flag will output a symbolic AST representation to stdout.

------------------

Makefile directives : 

- clean : only deletes .cpp files
- distclean: deletes all produced files


If you make changes to ast.hpp, execute make distclean before re-compiling, as the makefile is not aware of changes in the ast.hpp code. 


Project structure : 
- types.hpp  : type related information 
- symbol.hpp/cpp : semantic analysis and the  - compilation symbol tables 
- semantic.cpp : semantic analysis for all AST nodes 
- printon.cpp : code used for printing contents of the AST
- parser.y : grammar implementation file and the main driving unit of the compiler
- libs.c : library functions implemented directly in C, they are then linked with the final llvm code
- lexer.l : lexical analysis in Flex 
- compile.cpp : all the compilation 
- ast.hpp : AST Node definitions 
- ast.cpp : required implemenations 

/pcl_examples contains test programs in the /correct folder, and some negative examples for semantic analysis in the /wrong folder. The script `grader.py` implements a simple execution and logging. 

lexer.cpp, parser.hpp/cpp are automatically derived files, lexer.hpp is needed to connect C with C++ utilities.

-------------------
Special thanks to [Lykourgos Mastorou](https://github.com/lykmast) for the many invaluable discussions during this project without which I don't think I would have been able to complete it. Thanks also to [Petros Tzathas](https://github.com/pettza) for all the help during my PL studies, and to [Christos Serafeidis](https://github.com/chriserafi) for kickstarting part of the project and putting most of the work for the semantic analysis. 
