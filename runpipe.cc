#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/wait.h>

std::vector<std::vector<std::string>> parseCommand(const std::string& commandLine) {
    std::vector<std::vector<std::string>> commands;
    std::istringstream stream(commandLine);
    std::string segment;
    std::vector<std::string> currentCommand;

    while (std::getline(stream, segment, '|')) {
        std::istringstream cmdStream(segment);
        std::string arg;
        currentCommand.clear();
        while (cmdStream >> arg) {
            currentCommand.push_back(arg);
        }
        commands.push_back(currentCommand);
    }
    return commands;
}

void execCommand(const std::vector<std::string>& command) {
    std::vector<char*> args;
    for (const auto& arg : command) {
        args.push_back(const_cast<char*>(arg.c_str()));
    }
    args.push_back(nullptr);

    execvp(args[0], args.data());
    perror("execvp"); // Only reached if execvp fails
    exit(EXIT_FAILURE);
}

int main() {
    std::string commandLine;
    std::cout << "Enter pipe command:\n";
    std::getline(std::cin, commandLine);

    auto commands = parseCommand(commandLine);
    std::vector<int> pfd(2 * (commands.size() - 1)); // Pipe file descriptors

    // Set up pipes
    for (size_t i = 0; i < commands.size() - 1; ++i) {
        if (pipe(pfd.data() + i*2) == -1) {
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
                dup2(pfd[(i-1)*2], STDIN_FILENO);
            }
            if (i < commands.size() - 1) {
                dup2(pfd[i*2+1], STDOUT_FILENO);
            }

            // Close all pipe fds in the child
            for (auto& fd : pfd) {
                close(fd);
            }

            execCommand(commands[i]);
        }
    }

    // Close all pipe fds in the parent
    for (auto& fd : pfd) {
        close(fd);
    }

    // Wait for all child processes
    int status = 0;
    while (wait(&status) > 0) {
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            return WEXITSTATUS(status);
        }
    }

    return 0;
}
