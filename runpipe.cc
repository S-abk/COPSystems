#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <vector>
#include <cstring>

// Function to tokenize the command line into commands and their arguments
std::vector<std::vector<char*>> parseCommand(char* commandLine) {
    std::vector<std::vector<char*>> commands;
    char* command = strtok(commandLine, "|"); // Tokenize by pipe character

    while (command != nullptr) {
        std::vector<char*> args;
        char* arg = strtok(command, " \n"); // Tokenize the command into arguments by spaces
        while (arg != nullptr) {
            args.push_back(arg);
            arg = strtok(nullptr, " \n");
        }
        if (!args.empty()) {
            args.push_back(nullptr); // execvp expects a NULL terminated array
            commands.push_back(args);
        }
        command = strtok(nullptr, "|");
    }

    return commands;
}

// Function to execute each command in its own process
void executeCommand(std::vector<char*>& args) {
    execvp(args[0], args.data());
    // If execvp returns, there was an error
    perror("execvp failed");
    exit(EXIT_FAILURE);
}

int main() {
    std::string input;
    std::cout << "Enter pipe command:\n";
    std::getline(std::cin, input);

    // Convert std::string to C-style string for strtok
    std::vector<char> cstr(input.c_str(), input.c_str() + input.size() + 1);
    
    auto commands = parseCommand(cstr.data());

    std::vector<int> fds(2 * (commands.size() - 1)); // Pipe file descriptors

    for (size_t i = 0; i < commands.size() - 1; ++i) {
        if (pipe(fds.data() + i*2) != 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    for (size_t i = 0; i < commands.size(); ++i) {
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) { // Child process
            if (i > 0) {
                dup2(fds[(i-1)*2], STDIN_FILENO); // Redirect input
            }
            if (i < commands.size() - 1) {
                dup2(fds[i*2+1], STDOUT_FILENO); // Redirect output
            }

            // Close all fds in the child
            for (size_t j = 0; j < fds.size(); ++j) {
                close(fds[j]);
            }

            executeCommand(commands[i]);
        }
    }

    // Close all fds in the parent
    for (size_t i = 0; i < fds.size(); ++i) {
        close(fds[i]);
    }

    // Wait for all children to exit
    int status = 0;
    while (wait(&status) > 0) {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            return WEXITSTATUS(status);
        }
    }

    return 0;
}
