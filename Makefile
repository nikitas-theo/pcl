.PHONY: clean distclean default

LLVMCONFIG=llvm-config
CXX=g++

CXXFLAGS= -Wall -Wno-reorder -Wno-switch `$(LLVMCONFIG) --cxxflags` -std=c++17 -g 
LDFLAGS=`$(LLVMCONFIG) --ldflags --system-libs --libs all`



# add DEBUG_B=--debug
#     DEBUG_L=--debug


default: pcl

lexer.cpp: lexer.l
	flex -s  $(DEBUG_L) -o lexer.cpp lexer.l

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

lexer.o: lexer.cpp lexer.hpp parser.hpp

parser.cpp parser.hpp : parser.y
	bison -dv $(DEBUG_B) --report=lookahead -o parser.cpp parser.y


parser.o: parser.cpp lexer.hpp


pcl: lexer.o parser.o symbol.o ast.o
	$(CXX)  -o $@ $(CXXFLAGS) $^ $(LDFLAGS) 

clean: 
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	${MAKE} -C $(SYM_DIR) clean
	$(RM) pcl
