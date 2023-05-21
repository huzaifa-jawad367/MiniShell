#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <vector>

#include <sys/types.h>
#include <sys/wait.h>


#define TOKENSIZE 100


using namespace std;
void  StrTokenizer(char *line, char **argv);
void  myExecvp(char **argv);
void GetEnv();
void PrintHistory();

vector<string> history;


int main() {
    char *path2;
    char *arr[250];
    char *Tokenized;
    char input[250];
    char *argv[TOKENSIZE];

    while (true) {
        cout << "cwushell-> ";
        cin.getline(input, 250);
		history.push_back(input);
        StrTokenizer(input, argv);
         if (strcmp(argv[0], "exit") == 0) {
            break;
        } else if (strcmp(input, "\n") == 0) {
            continue;
        } else if (strcmp(argv[0], "cd") ==0){
            if(argv[1] != nullptr){
                if(chdir(argv[1]) != 0){
                    perror("chdir");
                } else{
                    cout << "Directory changed." << endl;
                }
            }
        } 
        else {
            myExecvp(argv);
        }
    }
    return 0;
}

void myExecvp(char **argv) {
    pid_t pid;
    int status;
    int childStatus;
    pid = fork();
    if (pid == 0) {
        bool appendMode = false;
        char* outFile = nullptr;
        int pipefd[2] = {0, 0}; // Pipe file descriptors

		// History function being called
		if (strcmp(argv[0], "history") == 0)
		{
			PrintHistory();
			return;
		}

        // Check for '>>' operator
        for (int i = 0; argv[i] != NULL; i++) {
            if (strcmp(argv[i], ">>") == 0) {
                appendMode = true;
                outFile = argv[i + 1];
                argv[i] = nullptr;
                break;
            }
        }

        if (appendMode) {
            std::fstream file(outFile, std::ios::app); // Open file in append mode
            std::string input;

            // Read user input until termination string is entered
            while (true) {
                std::getline(std::cin, input);
                if (std::cin.eof()) {
                    break;
                }
                file << input << std::endl;
            }
            file.close();
            exit(0);
        } else {
            // Check for pipe operator '|'
            for (int i = 0; argv[i] != nullptr; i++) {
                if (strcmp(argv[i], "|") == 0) {
                    argv[i] = nullptr;

                    // Create pipe
                    if (pipe(pipefd) == -1) {
                        perror("pipe");
                        exit(1);
                    }

                    pid_t childpid;
                    childpid = fork();
                    if (childpid == 0) {
                        // Child process

                        // Close read end of the pipe
                        close(pipefd[0]);

                        // Duplicate the write end of the pipe to stdout
                        dup2(pipefd[1], STDOUT_FILENO);
                        close(pipefd[1]);

                        execvp(argv[0], argv);
                        perror("execvp");
                        exit(1);
                    } else if (childpid > 0) {
                        // Parent process

                        // Close write end of the pipe
                        close(pipefd[1]);

                        // Duplicate the read end of the pipe to stdin
                        dup2(pipefd[0], STDIN_FILENO);
                        close(pipefd[0]);

                        execvp(argv[i + 1], &argv[i + 1]);
                        perror("execvp");
                        exit(1);
                    } else {
                        perror("fork");
                        exit(1);
                    }
                }
            }

              // Check for 'cd' command
            if (strcmp(argv[0], "cd") == 0) {
                if (argv[1] != nullptr) {
                    if (chdir(argv[1]) != 0) {
                        perror("chdir");
                    }
                } else {
                    cout << "Usage: cd <directory>" << endl;
                }
                exit(0);
            }

            // Execute a single command if no pipe is present
            execvp(argv[0], argv);
            perror("execvp");
			
            exit(1);
        }
    } else if (pid < 0) {
        cout << "something went wrong!" << endl;
    } else {
        int status;
        waitpid(pid, &status, 0);
    }

}


void StrTokenizer(char *input, char **argv) {
    char *stringTokenized;
    stringTokenized = strtok(input, " ");
    while(stringTokenized != NULL)
    {
        *argv++  = stringTokenized;
        stringTokenized = strtok(NULL, " ");
    }

    *argv = NULL;
}

void GetEnv()
{
	char *path2;
	char *arr2[250];
	char *Tokenized ;
	path2 = getenv("PATH");
	Tokenized = strtok(path2, ":");
	int k = 0;
	while(Tokenized != NULL)
	{
		arr2[k] = Tokenized;
		Tokenized = strtok(NULL, ":");
		k++;
	}
	arr2[k] = NULL;
}

void PrintHistory() {
    int start = (history.size() > 10) ? (history.size() - 10) : 0;
    for (size_t i = start; i < history.size(); i++) {
        std::cout << history[i] << std::endl;
    }
}