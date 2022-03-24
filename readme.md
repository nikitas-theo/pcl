# A compiler for the PCL language

The PCL language is a strict subset of the pascal programming language, enriched by small extra features. Complete definition given (in Greek) in the definition pdf. Some basic features of the language: 

* Syntax similar to Pascal
* Scoping as Pascal
* Integer, boolean, real numbers and character primitive types
* Arrays with fixed or unknown size and dynamic memmory allocation
* A set of standard library functions 



A key challenge when desining a compiler for the PCL language is the use of static scoping as in the Pascal language. Function definitions can be nested and a function `f` can define another funciton `g` in it's local variables; the function `g` can then access all variables in the outer scopes enclosing it. This is a challenging feature to implement in llvm IR because of the C-style structure of llvm functions. Different solutions where discussed for the problem e.g. [here](https://stackoverflow.com/questions/55736390/llvm-how-to-make-a-nested-function-see-an-outside-functions-variables) or [here](https://www.reddit.com/r/LLVM/comments/g7qcwo/nested_scopes/?utm_source=share&utm_medium=web2x&context=3).  

In the end I opted for the solution detailed in the text ([link](https://www.theseus.fi/bitstream/handle/10024/166119/Nguyen_Anh%20.pdf;jsessionid=97A6A61F9CF3E811E7BBB6F4A5E86EAA?sequence=2)) by Anh Nguyen in paragraph 6.2.9 - Nested functions. Essentially each function carries a hidden argument which is a struct with any extra variables needed from an outside scope, the hidden struct type is defined at compile time. This seemed the most elegant, generally applicable and true to the language semantics solution. 

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

/src
- types.hpp  : type related information 
- symbol.hpp/cpp : semantic analysis and compilation symbol tables 
- semantic.cpp : semantic analysis for all AST nodes 
- printon.cpp : code used for printing contents of the AST
- parser.y : grammar implementation file and the main driving unit of the compiler
- libs.c : library functions implemented directly in C, they are then linked with the final llvm code
- lexer.l : lexical analysis in Flex 
- compile.cpp :  compilation for all AST noted using the IRBuilder interface 
- ast.hpp : AST Node definitions 
- ast.cpp : other implemenations 

/pcl_examples 

contains test programs in the /correct folder, and some negative examples for semantic analysis in the /wrong folder. The script `grader.py` implements a simple grader with execution and compilation logs.

lexer.cpp, parser.hpp/cpp are automatically derived files, lexer.hpp is needed to connect C with C++ utilities.

-------------------
Special thanks to: 
* [Lykourgos Mastorou](https://github.com/lykmast) for the many invaluable discussions during this project. 
* [Petros Tzathas](https://github.com/pettza) for all the help during my studies on programming languages
* [Christos Serafeidis](https://github.com/chriserafi) for co-starting the project and work on semantic analysis. 
