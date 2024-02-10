#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main() {
    int pipe_fds[2]; /* Pipe file descriptors */
    pid_t child1_pid, child2_pid; /* Process IDs for child processes */

    if (pipe(pipe_fds) == -1) { /* Initialize pipe */
        perror("pipe error");
        exit(EXIT_FAILURE);
    }

    child1_pid = fork(); /* Fork first child process */
    if (child1_pid == -1) {
        perror("fork error");
        exit(EXIT_FAILURE);
    }

    if (child1_pid == 0) { /* In first child process */
        dup2(pipe_fds[1], STDOUT_FILENO); /* Redirect standard output to pipe's write end */
        close(pipe_fds[0]); /* Close pipe's read end */
        close(pipe_fds[1]); /* Close write end after duplication */
        execlp("ps", "ps", "-aux", NULL); /* Execute 'ps -aux' command */
        perror("execlp error with ps");
        exit(EXIT_FAILURE);
    } else {
        child2_pid = fork(); /* Fork second child process */
        if (child2_pid == -1) {
            perror("fork error");
            exit(EXIT_FAILURE);
        }

        if (child2_pid == 0) { /* In second child process */
            dup2(pipe_fds[0], STDIN_FILENO); /* Redirect standard input to pipe's read end */
            close(pipe_fds[1]); /* Close pipe's write end */
            close(pipe_fds[0]); /* Close read end after duplication */
            execlp("sort", "sort", "-r", "-n", "-k", "5", NULL); /* Execute 'sort -r -n -k 5' command */
            perror("execlp error with sort");
            exit(EXIT_FAILURE);
        } else {
            /* In parent process */
            close(pipe_fds[0]); /* Close pipe's read end */
            close(pipe_fds[1]); /* Close pipe's write end */
            waitpid(child1_pid, NULL, 0); /* Wait for first child process to terminate */
            waitpid(child2_pid, NULL, 0); /* Wait for second child process to terminate */
        }
    }

    return 0; /* Terminate parent process */
}
