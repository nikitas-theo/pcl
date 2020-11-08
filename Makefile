CC=g++
FLAGS= -Wall -pedantic -std=c++11

.PHONY: clean

default: pcl

SYM_DIR= symbol


OBJ_SYM= $(patsubst %.c,%.o,$(wildcard ${SYM_DIR}/*.c)) 

$(OBJ_SYM):
		${MAKE} -C $(SYM_DIR) objfiles

ifdef db
DEBUG_B=--debug
endif
ifdef df
DEBUG_F=--debug
endif

#	$< :  first prerequisite 
#	$^ :  all prerequisites 
#   $@ :  target 

parser.cpp parser.hpp : parser.y
	bison -dv $(DEBUG_B) $< --report=lookahead -o parser.cpp

lex.yy.c : lexer.l
	flex $(DEBUG_F) $<
pcl: lex.yy.c parser.cpp $(OBJ_SYM) | parser.hpp
	$(CC)  $(OBJ_SYM)  -o $@  lex.yy.c parser.cpp -lfl $(FLAGS) 
clean:
	rm lex.yy.c parser.cpp parser.hpp parser.output pcl && ${MAKE} -C $(SYM_DIR) clean

