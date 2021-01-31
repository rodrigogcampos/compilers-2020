CFLAGS = -g -I./include

mybc: mybc.o lexer.o parser.o keywords.o
	$(CC) -o $@ $^

clean:
	$(RM) *.o

mostlyclean: clean
	$(RM) *~
