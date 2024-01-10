## Simple Unix Shell
### Naman Birla and Pranav Bharadwaj

## Project Details
The working of this project requires a LINUX based machine.

Implementing Simple-Shell that waits for user input, executes commands provided in the user 
input,  and  then  repeats  until  terminated  using  Ctrl-C. This simpleshell can also read and execute bash script written in an external file. The shell executes simple linux commands. The simple shell also supports the ```history``` command and also displays the process details upon exitting.

The shell CANNOT execute commands like ```cd``` , i.e. the commands whose executables are not present in the ```bin``` folder of the unix machine.


## How to run
A linux based machine is required to run the simple-shell.

The Makefile conatains the appropriate code required for the working of the simpleshell. It also cleans them internally after exitting the simpleshell.

1. Open the project in the terminal and run the command ```make```. 

It compiles the ```helloworld.c```, ```fib.c```  and  ```main.c``` files to produce the executables ```helloworld```, ```fib``` and ```main``` respectively.

2. If you want to run and execute a shell script written in an external file, run the command ```./main <filename>```, replacing ```<filename>``` with appropriate filename.

3. If you want to run the simpleshell, just run the command ```./main``` without any arguments.

To use the simpleshell again repeat the above steps.

The executables created are internally cleaned when the program ends.



## Contributions
The general overview of the contributions:

Naman Birla (n4m4n-3irl4):
1.	Commands with piping
2.	Signal handling and printing additional details of each command
3.	Error handling

Pranav Bharadwaj:
1.	Commands without piping
2.	Reading commands from an external file and history command
3.	Error handling

