.PHONY: clean distclean default

LLVMCONFIG=llvm-config
CXX=g++

CXXFLAGS= -Wall -Wno-reorder -Wno-switch `$(LLVMCONFIG) --cxxflags` -std=c++17 -g 
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`

default: pcl

# General object file production rule --
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<
# --------------------------------------

# Lexer --------------------------------
lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

lexer.o: lexer.cpp lexer.hpp parser.hpp
# --------------------------------------

# Parser -------------------------------
parser.cpp parser.hpp : parser.y
	bison -dv --report=lookahead -o parser.cpp parser.y

parser.o: parser.cpp lexer.hpp
# --------------------------------------

# AST object files production rules ----
ast%.o: ast%.cpp ast.hpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -c $<
# --------------------------------------

# PCL COMPILER -------------------------
pcl: lexer.o parser.o symbol.o ast_compile.o ast_semantic.o ast_helpers.o ast_printon.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)
# --------------------------------------

# Cleaning -----------------------------
# $(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o
# ${MAKE} -C $(SYM_DIR) clean
clean: 
	$(RM) symbol.o ast_compile.o ast_semantic.o ast_helpers.o ast_printon.o

distclean: clean
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output lexer.o parser.o
	$(RM) pcl
