# Overview
This is a custom Unix-like shell implemented in C. It supports executing system programs such as ```ls```, ```grep```, ```cat```, and others, using ```fork()``` and ```exec()``` system 
calls. The shell interface displays the current working directory and user in the prompt in the format:<br/>
```csd5260-hy345sh@kotsos: /current/directory/path```
<br/>

## Supported Features
- Execution of external commands
- Input and output redirection using:
 - ```>``` and ```>>``` for file output (overwrite and append)
 - '''<''' and '''<<''' for file input
- Dynamic prompt with username and current working directory

## Not Implemented:
- Internal commands such as ```cd``` for directory navigation
- Command piping (```|```) for chaining multiple commands

# Build Instructions
To compile the shell, run:<br/>
```make all```
<br/>
This will generate the executable ```hy345sh``` in the current directory. 

This project was developed as part of an academic assignment for the HY345 (Operating Systems) course at the University of Crete. It focuses on understanding process creation, file 
descriptor manipulation, and command parsing in a Unix environment.

