CC = g++
CFLAGS = -Wall -Werror -ansi -pedantic

#creates an executable rshell
all: rshell

rshell: src/hw3-sig.cpp bin
	$(CC) $(CFLAGS) src/hw3-sig.cpp  -o rshell &&  mv rshell bin

ls: src/hw1-ls.cpp bin
	$(CC) $(CFLAGS) src/hw1-ls.cpp -o ls &&  mv ls  bin

rshell0: src/hw0-rshell.cpp bin
	$(CC) $(CFLAGS) src/hw0-rshell.cpp  -o rshell &&  mv rshell bin

bin: 
	mkdir bin;

#removes bin and executables rshell and ls
clean:
	rm -r bin;
