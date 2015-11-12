CC = gcc
CFLAGS = -g -Wall  
OBJECTS = main.o \
lexemes.o \
lexical_analyzer.o  \
syntax_analyzer.o 
INCLUDE = -I ../include/compiler/
LIBS = -L ../lib

all : markdown_compiler

markdown_compiler : $(OBJECTS)
	$(CC) -o markdown_compiler $(OBJECTS) $(LIBS) -lcdatastructures

main.o : main.c ../include/compiler/lexical_analyzer.h ../include/compiler/syntax_analyzer.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c

lexemes.o : lexemes.c ../include/compiler/lexemes.h
	$(CC) $(CFLAGS) $(INCLUDE) -c lexemes.c

lexical_analyzer.o : lexical_analyzer.c ../include/compiler/lexical_analyzer.h ../include/compiler/lexemes.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c lexical_analyzer.c

syntax_analyzer.o : syntax_analyzer.c ../include/compiler/syntax_analyzer.h ../include/compiler/lexical_analyzer.h ../include/compiler/lexemes.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c syntax_analyzer.c

clean :
	rm -f *.o markdown_compiler
