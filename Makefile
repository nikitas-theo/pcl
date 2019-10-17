CC=g++
FLAGS=Wall

default: pcl

ifdef db
DEBUG_B=--debug
endif
ifdef df
DEBUG_F=--debug
endif

parser.cpp parser.hpp : parser.y
	bison -dv $(DEBUG_B) parser.y --report=lookahead -o parser.cpp

lex.yy.c : lexer.l
	flex $(DEBUG_F)  lexer.l
pcl: lex.yy.c parser.cpp | parser.hpp
	$(CC)  lex.yy.c parser.cpp  -o pcl -lfl -$(FLAGS)
clean:
	rm lex.yy.c parser.cpp parser.hpp parser.output pcl
