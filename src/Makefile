CC = gcc
CFLAGS = -g -Wall  
OBJECTS = main.o \
lexemes.o \
lexical_analyzer.o  \
syntax_analyzer.o \
file_manager.o \
semantic_analyzer.o \
hash.o
INCLUDE = -I ../include/ds/ -I ../include/compiler/
LIBS = -L ../lib -lcdatastructures

all : markdown_compiler

markdown_compiler : $(OBJECTS)
	$(CC) -o markdown_compiler $(OBJECTS) $(LIBS) 

main.o : main.c ../include/compiler/lexical_analyzer.h ../include/compiler/syntax_analyzer.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c main.c

lexemes.o : lexemes.c ../include/compiler/lexemes.h
	$(CC) $(CFLAGS) $(INCLUDE) -c lexemes.c

file_manager.o : file_manager.c ../include/compiler/file_manager.h
	$(CC) $(CFLAGS) $(INCLUDE) -c file_manager.c

lexical_analyzer.o : lexical_analyzer.c ../include/compiler/lexical_analyzer.h ../include/compiler/lexemes.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c lexical_analyzer.c

syntax_analyzer.o : syntax_analyzer.c ../include/compiler/syntax_analyzer.h ../include/compiler/lexical_analyzer.h ../include/compiler/lexemes.h ../include/compiler/main.h
	$(CC) $(CFLAGS) $(INCLUDE) -c syntax_analyzer.c

semantic_analyzer.o : semantic_analyzer.c ../include/compiler/semantic_analyzer.h ../include/compiler/lexemes.h ../include/compiler/main.h ../include/ds/hash.h
	$(CC) $(CFLAGS) $(INCLUDE) -c semantic_analyzer.c

hash.o : hash.c ../include/ds/hash.h
	$(CC) $(CFLAGS) $(INCLUDE) -c hash.c

clean :
	rm -f *.o markdown_compiler
