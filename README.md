# Basic Command Shell
===
When run, command prompts are printed, and commands are read in from the user one line at a time and executed.
Commands should be given like so
```
executable [ argumentList ] [ connector command ]
```
Where connectors can be `;`, `&&`, or `||`.
To exit the program, the user types `exit` at the beginning of a command. Any single `&` or `|` will be ignored.
Anything following a `#` is considered a comment, and thus will be ignored.

## Installation
---
```
$ git clone  https://github.com/yourusername/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```


##Bugs/Limitations/Issues
---
1. If the usr input has a `|` or a `&` at the beginning, it causes a segmentation fault and quits the program.
2. If a command fails, and exit is inputed for the next command prompt, the line will be ignored.
3. The program will only hold up to 99 arguments for every command.




