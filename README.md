# Basic Command Shell

When run, command prompts are printed, and commands are read in from the user one line at a time and executed.
This file first outlines how to run the program, then gives details on how it works, then give the bugs and problems with this programm.

## Installation
The following is the instructions for running the command shell
```
$ git clone  https://github.com/yourusername/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```
Running `make` creates the executable and places it into a created directory `bin`. Running `make clean` will delete the directory `bin` and everything in it.

## Details
One the program has be run, commands should be given like so
```
executable [ argumentList ] [ connector command ]
```
Where connectors can be `;`, `&&`, or `||`. The `executable` is an executable program in the `PATH`.
To exit the program, the user types `exit` at the beginning of a command.
When exit is found, anything following it will not execute.
Any single `&` or `|` will be ignored.
Anything following a `#` is considered a comment, and thus will be ignored.
The directory `tests` holds a `exec.script` file with test cases.
For connectors, if a command will not be executed due to the connector before it, all other commands after will also not execute. For example
```
true || ls && pwd
```
will only execute the first command (here `true`), but because it suceeded, 
and thus `ls` won't be executed, then `pwd` will also not be executed.

##Bugs/Limitations/Issues
---
1. If the user input has only tabs or spaces before a `|` or a `&` at the beginning, before any commands, it causes a segmentation fault and quits the program.
examples:
```
  || ls
 & ps
```

2. The program will only hold up to 99 arguments for every command.
