CC = g++
CFLAGS = -Wall -Werror -ansi -pedantic

#creates an executable rshell
all: rshell

rshell: src/hw0-rshell.cpp
	$(CC) $(CFLAGS) src/hw0-rshell.cpp  -o rshell && mkdir bin && mv rshell bin

#removes bin and executable rshell
clean:
	rm -r bin;
