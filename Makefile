CC = gcc
CFLAGS = -g -Wall 
OBJECTS = main.o \
lexical_analyzer.o  \

all : markdown_compiler

markdown_compiler : $(OBJECTS)
	$(CC) -o markdown_compiler $(OBJECTS) 

main.o : main.c lexical_analyzer.h main.h
	$(CC) $(CFLAGS) -c main.c

lexical_analyzer.o : lexical_analyzer.c lexical_analyzer.h lexemes.h main.h
	$(CC) $(CFLAGS) -c lexical_analyzer.c

clean :
	rm -f *.o markdown_compiler
