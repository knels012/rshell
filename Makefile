CC = g++
CFLAGS = -Wall -Werror -ansi -pedantic

#creates an executable rshell
all: rshell

rshell: src/hw0-rshell.cpp
	mkdir bin && $(CC) $(CFLAGS) src/hw0-rshell.cpp  -o rshell && mv rshell bin

#removes bin and executable rshell
clean:
	rm bin/rshell && rmdir bin
