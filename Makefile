CFLAGS=-ansi -Wall -pedantic -g

bin/wyog: bin src/*.c
	$(CC) $(CFLAGS) $(LDFLAGS) -o bin/wyog src/*.c

bin:
	mkdir bin
