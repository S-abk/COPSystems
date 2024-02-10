#include <unistd.h>
#include <sys/wait.h>
#include <iostream>

int main() {
    int pipefd[2];
    pid_t cpid1, cpid2;

    // Create a pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Create first child process
    cpid1 = fork();
    if (cpid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid1 == 0) { // Child 1
        // Redirect stdout to the write end of the pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]); // Close the read end, not needed here
        close(pipefd[1]); // Close original write end after dup

        // Execute "ps -aux"
        execlp("ps", "ps", "-aux", NULL);
        // If execlp returns, it must have failed
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Create second child process
        cpid2 = fork();
        if (cpid2 == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid2 == 0) { // Child 2
            // Redirect stdin from the read end of the pipe
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[1]); // Close the write end, not needed here
            close(pipefd[0]); // Close original read end after dup

            // Execute "sort -r -n -k 5"
            execlp("sort", "sort", "-r", "-n", "-k", "5", NULL);
            // If execlp returns, it must have failed
            perror("execlp");
            exit(EXIT_FAILURE);
        } else {
            // Parent process
            close(pipefd[0]); // Close the read end
            close(pipefd[1]); // Close the write end

            // Wait for both child processes to terminate
            waitpid(cpid1, NULL, 0);
            waitpid(cpid2, NULL, 0);
        }
    }

    return 0;
}
