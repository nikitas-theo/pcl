.PHONY: clean distclean default

LLVMCONFIG=llvm-config
CXX=g++

CXXFLAGS= -Wall -Wno-reorder -Wno-switch `$(LLVMCONFIG) --cxxflags` -std=c++17 -g
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`

default: pcl

# General object file production rule --
%.o: %.cpp | ast.hpp
	$(CXX) $(CXXFLAGS) -c $<
# --------------------------------------

# Lexer --------------------------------
lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

# lexer.o: lexer.cpp parser.hpp
	
# Parser -------------------------------
parser.cpp parser.hpp : parser.y
	bison -dv --report=lookahead -o parser.cpp parser.y

# PCL COMPILER -------------------------
pcl: parser.o lexer.o symbol.o compile.o semantic.o helpers.o printon.o error.o
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS)
# --------------------------------------

# Cleaning -----------------------------
# $(RM) lexer.cpp parser.cpp parser.hpp parser.output
# ${MAKE} -C $(SYM_DIR) clean
clean: 
	$(RM) symbol.o compile.o semantic.o helpers.o printon.o error.o

distclean: clean
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output lexer.o parser.o
	$(RM) pcl
