CC = g++
CFLAGS = -Wall -Werror -ansi -pedantic

#creates an executable rshell
all: rshell ls

ls: src/hw1-ls.cpp
	$(CC) $(CFLAGS) src/hw1-ls.cpp -o ls && mkdir bin; mv ls  bin

rshell: src/hw0-rshell.cpp
	$(CC) $(CFLAGS) src/hw0-rshell.cpp  -o rshell && mkdir bin;  mv rshell bin


#removes bin and executables rshell and ls
clean:
	rm -r bin;
