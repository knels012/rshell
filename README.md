# Basic Command Shell

When run, command prompts are printed, and commands are read in from the user one line at a time and executed.
This file first outlines how to run the program, then gives details on how it works, then give the bugs and problems with this programm.

## Installation

```
$ git clone  https://github.com/yourusername/rshell.git
$ cd rshell
$ git checkout hw0
$ make
$ bin/rshell
```

## Details
One the program has be run, commands should be given like so
```
executable [ argumentList ] [ connector command ]
```
Where connectors can be `;`, `&&`, or `||`. The `executable` is an executable program in the `PATH`.
To exit the program, the user types `exit` at the beginning of a command. Any single `&` or `|` will be ignored.
Anything following a `#` is considered a comment, and thus will be ignored.
The directory `tests` holds a `exec.script` file with test cases.
For connectors, if a command will not be executed due to the connector before it, all other commands after will also not execute. For example
```
true || ls && pwd
```
will only execute the first command (here `true`), but becuase it suceeded, and thus `ls` won't be executed, then `pwd`, will also not be executed.

## Bugs/Limitations/Issues

1. If the usr input has a `|` or a `&` at the beginning, it causes a segmentation fault and quits the program.
2. If a command fails, and exit is inputed for the next command prompt, the line will be ignored.
3. The program will only hold up to 99 arguments for every command.




