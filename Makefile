.PHONY: clean distclean default

LLVMCONFIG=llvm-config
CXX=g++

CXXFLAGS= -Wall -Wno-reorder -Wno-switch `$(LLVMCONFIG) --cxxflags` -std=c++17
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`

SYM_DIR= symbol
OBJ_SYM= $(patsubst %.c,%.o,$(wildcard ${SYM_DIR}/*.c)) 

# add DEBUG_B=--debug
#     DEBUG_L=--debug


default: pcl

$(OBJ_SYM):
	${MAKE} -C $(SYM_DIR) objfiles

lexer.cpp: lexer.l
	flex -s -o lexer.cpp lexer.l

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

lexer.o: lexer.cpp lexer.hpp parser.hpp

parser.cpp parser.hpp : parser.y
	bison -dv $(DEBUG_B) --report=lookahead -o parser.cpp parser.y

ast.o : ast.hpp ast.cpp 
parser.o: parser.cpp lexer.hpp

symbol_compatible.o: symbol_compatible.cpp symbol_compatible.hpp

pcl: lexer.o parser.o symbol_compatible.o $(OBJ_SYM) ast.o
	$(CXX)  -o $@ $(CXXFLAGS) $^ $(LDFLAGS) 

clean: 
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	${MAKE} -C $(SYM_DIR) clean
	$(RM) pcl
