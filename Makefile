CC = gcc
CFLAGS = -g -Wall  
OBJECTS = main.o \
lexemes.o \
lexical_analyzer.o  \

all : markdown_compiler

markdown_compiler : $(OBJECTS)
	$(CC) -o markdown_compiler $(OBJECTS) -L ./lib -lcdatastructures

main.o : main.c lexical_analyzer.h main.h
	$(CC) $(CFLAGS) -c main.c

lexemes.o : lexemes.c lexemes.h
	$(CC) $(CFLAGS) -c lexemes.c

lexical_analyzer.o : lexical_analyzer.c lexical_analyzer.h lexemes.h main.h
	$(CC) $(CFLAGS) -c lexical_analyzer.c

clean :
	rm -f *.o markdown_compiler
