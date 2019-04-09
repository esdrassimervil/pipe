#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define K	4096

void parent(pid_t pid, int pipefd[2], char*filename)
{
  int r, f, w, t;
  char buf[K];
  int status = 0;
 
   f = open(filename, O_RDONLY);
   if (f < 0){
     perror("open");
     exit(0); 
  }
  close(pipefd[0]);
      //Read up to buf number of bytes from the file descriptor (f)
      while((r = read(f, buf, K)) > 0){
        //Make sure every byte read is written to the pipe (pipefd[1])
       t = 0;
       do { 
        w = write(pipefd[1], buf+t, r-t);
        t += w;
       } while (t < r); 
     } if (r < 0) perror("read");
    
     //Wait for the child to complete then print the child's exit status
    close(pipefd[1]);
     //printf("Waiting for the child to complete...\n");
     while(waitpid(pid, &status, 0) < 0) perror("wait"); 
      printf("Child exited with %d exit status\n",  WEXITSTATUS(status));
}

void child(int pipefd[2])
{
  int f;
  char *arg[2] = {"/usr/games/ppt", NULL};
   
  close(pipefd[1]);
    //Replaces the stdin of the child with the read end of the pipe 
    if(dup2(pipefd[0], 0) == -1){
       perror("dup2");
       exit(EXIT_FAILURE);
    }
    close(pipefd[0]);
    
    //Replaces the stdout of the child so that it outputs to the file "output.txt" (f)
    f = open("output.txt", O_CREAT | O_WRONLY | O_TRUNC, 0666);
    if(dup2(f, 1) == -1){
       perror("dup2");
       exit(EXIT_FAILURE);   
    }
    close(f);
    execve("/usr/games/ppt", arg, NULL);
    perror("execve");
}



int main(int argc, char *argv[])
{
  int pipefd[2];
  // Ask for a file as the second argument
  if (argc < 2) {
    printf("Usage: pipe <filename>\n");
    return -1;
}

  //creates a pipe
  if (pipe(pipefd) < 0){
     perror("pipe");
     return -1;
  }
  
  //forks creates a child process
  pid_t pid = fork();
  if (pid < 0){
    perror("fork");
    return -1;
  }
  
  if (pid > 0) parent(pid,pipefd,argv[1]);
  else child(pipefd);
      return 0;
}