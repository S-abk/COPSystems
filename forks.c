#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h> // Include for wait()
#include <errno.h>

int main(){
    pid_t pid;
    pid = fork();
    
    if (pid < 0) {
        fprintf(stderr, "ERROR: fork failed with code %d \n", errno);
        return 1;
    }
    else if (pid == 0) {
        printf("CHILD: my pid=%d \n\n", getpid());
        printf("CHILD: Now starting new command with exec: \n\n");
        
        // exec NEVER returns if the program starts with OK:
        execlp("ls", "ls", "-l", NULL);
        
        // ERROR if we got to this point:
        fprintf(stderr, "CHILD: ERROR: exec failed with code %d\n", errno);
        return -2;
    }
    else { // Parent (Initial) process, pid is the child's pid:
        printf("\n\nParent (pid=%d): child process created with pid=%d\n\n", getpid(), pid);
        
        // wait for child to end; get child return error code:
        int retval = 0;
        pid_t childpid = wait(&retval);
        
        printf("Parent (my pid=%d): child process with pid=%d returned with error code=%d \n\n", getpid(), childpid, WEXITSTATUS(retval));
    }
    printf("OUTSIDE IF: my pid=%d \n\n", getpid());
    return 0; //0 is the "normal" return code in case of no errors
}
