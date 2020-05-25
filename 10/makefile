CC=g++
EXTRAFLAGS = -std=gnu++17

run: lexer.o grammar.o parser.o interpreter.o interface.o 
	$(CC) $(EXTRAFLAGS) -o run lexer.o grammar.o parser.o interpreter.o interface.o

test: lexer.o grammar.o parser.o interpreter.o test.o
	$(CC) $(EXTRAFLAGS) -o test lexer.o grammar.o parser.o interpreter.o test.o

interface.o: interface.cpp
	$(CC) $(EXTRAFLAGS) -c interface.cpp

test.o: test.cpp
	$(CC) $(EXTRAFLAGS) -c test.cpp

interpreter.o: interpreter.cpp interpreter.h
	$(CC) $(EXTRAFLAGS) -c interpreter.cpp

parser.o: parser.cpp parser.h lexeme.h grammar.h
	$(CC) $(EXTRAFLAGS) -c parser.cpp

grammar.o: grammar.cpp grammar.h lexeme.h
	$(CC) $(EXTRAFLAGS) -c grammar.cpp

lexer.o: lexer.cpp lexer.h lexeme.h
	$(CC) $(EXTRAFLAGS) -c lexer.cpp

clean:
	rm -rf *.o parse
