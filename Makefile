CC = g++
CFLAGS = -Wall -Werror -ansi -pedantic

#creates an executable rshell
all: rshell

ls: src/hw1-ls.cpp bin
	$(CC) $(CFLAGS) src/hw1-ls.cpp -o ls &&  mv ls  bin

rshell: src/hw2-rshell.cpp bin
	$(CC) $(CFLAGS) src/hw2-rshell.cpp  -o rshell &&  mv rshell bin

bin: 
	mkdir bin;

#removes bin and executables rshell and ls
clean:
	rm -r bin;
