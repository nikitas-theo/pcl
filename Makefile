.PHONY: clean distclean default

CXX=g++
CXXFLAGS=-Wall -std=c++11 -pedantic -g
LDFLAGS=

SYM_DIR= symbol
OBJ_SYM= $(patsubst %.c,%.o,$(wildcard ${SYM_DIR}/*.c)) 

ifdef db
DEBUG_B=--debug
endif
ifdef df
DEBUG_F=--debug
endif

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

parser.o: parser.cpp lexer.hpp

pcl: lexer.o parser.o $(OBJ_SYM)  ast.hpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean: 
	$(RM) lexer.cpp parser.cpp parser.hpp parser.output *.o

distclean: clean
	${MAKE} -C $(SYM_DIR) clean
	$(RM) pcl
