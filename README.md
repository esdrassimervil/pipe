# pipe
program is essentially cat filename | ppt > output.txt
Takes one command line parameter, an input filename.
Creates a pipe(2) to communicate with a child process.
Uses fork(2) to create a child process.
Replaces the stdin of the child with the read end of the pipe (using dup2(2))
Replaces the stdout of the child so that it outputs to the file "output.txt" (using dup2(2))
execve(2)'s in the child, the program: /usr/games/ppt (no command line arguments should be passed to ppt.)
The parent process should then feed the contents of the input file (name given on the command line) to the child process using a read/write loop.
The parent should finally wait for the child to complete using the waitpid(2) call and print the child's exit status.
