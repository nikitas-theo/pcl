CC=g++
FLAGS=Wall

default: pcl

parser.cpp parser.hpp : parser.y
	bison -dv parser.y -o parser.cpp 
lex.yy.c : lexer.l 
	flex  lexer.l 
pcl: lex.yy.c parser.cpp | parser.hpp
	$(CC)  lex.yy.c parser.cpp  -o pcl -lfl -$(FLAGS)
clean: 
	rm lex.yy.c parser.cpp parser.hpp parser.output pcl 
