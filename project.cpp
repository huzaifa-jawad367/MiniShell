#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/wait.h>


#define TOKENSIZE 100


using namespace std;
void  StrTokenizer(char *line, char **argv);
void  myExecvp(char **argv);
void GetEnv();
void PrintHistory();
void Grep(string filePath, string word);
bool isWordInSentence(const string& sentence, const string& word);

vector<string> history;


int main() {
    char *path2;
    char *arr[250];
    char *Tokenized;
    char input[250];
    char *argv[TOKENSIZE];

    while (true) {
        cout << "cwushell-> ";
        cin.getline(input, 250); //get user input
		history.push_back(input);
        StrTokenizer(input, argv); //tokenize user input, store in argv array
        if (strcmp(argv[0], "exit") == 0) {
            break;
        } else if (strcmp(input, "\n") == 0) {
            continue;
        } else if (strcmp(argv[0], "cd") ==0){ //if 'cd' command-> execute chdir() system call
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
    if (pid == 0) { //child process runs the command entered by user
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
        
                //open system call used, file opened in write only + append mode
                //created if does not exist
                int fileDescriptor = open(outFile, O_WRONLY | O_CREAT | O_APPEND, 0644); 
                if (fileDescriptor == -1) {
                    perror("open");
                    exit(1);
                }

                // Read user input until termination string is entered
                while (true) {
                    std::string input;
                    std::getline(std::cin, input);
                    if (std::cin.eof()) {
                        break;
                    }
                    input += '\n';
                    
                    //input.c_str() passes pointer to character array of user input
                    ssize_t bytesWritten = write(fileDescriptor, input.c_str(), input.length());
                    if (bytesWritten == -1) {
                        perror("write");
                        close(fileDescriptor);
                        exit(1);
                    }
                }

            //close system call to close the file
                close(fileDescriptor);
                exit(0);
    }
        } else if (strcmp(argv[0], "grep") == 0) {
            // Call the Grep function for the "grep" command
            if (argv[1] != nullptr && argv[2] != nullptr) {
                Grep(argv[2], argv[1]);
            } else {
                cout << "Usage: grep <word> <file>" << endl;
            }
            exit(0);
        else {
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
                } 
                else if (childpid > 0) {
                    // Parent process

                    // Close write end of the pipe
                    close(pipefd[1]);

                    // Duplicate the read end of the pipe to stdin
                    dup2(pipefd[0], STDIN_FILENO);
                    close(pipefd[0]);

                    execvp(argv[i + 1], &argv[i + 1]);
                    perror("execvp");
                    exit(1);
                } 
                else {
                    perror("fork");
                    exit(1);
                }
            }
        }
         
            // Execute a single command if no pipe is present
            execvp(argv[0], argv);
            perror("execvp");

            exit(1);
        }
    } else if (pid < 0) {
        cout << "something went wrong!" << endl;
    }   
    else { // parent process
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
        cout << history[i] << endl;
    }
}


bool isWordInSentence(const string& sentence, const string& word) {
    // Convert the sentence and word to lowercase for case-insensitive comparison
    string lowercaseSentence = sentence;
    string lowercaseWord = word;
    
	// transform(lowercaseSentence.begin(), lowercaseSentence.end(), lowercaseSentence.begin(), ::tolower);
	transform(lowercaseSentence.begin(), lowercaseSentence.end(), lowercaseSentence.begin(), ::tolower);
    transform(lowercaseWord.begin(), lowercaseWord.end(), lowercaseWord.begin(), ::tolower);

    // Check if the word exists in the sentence
    size_t found = lowercaseSentence.find(lowercaseWord);
    return (found != string::npos);
}

void Grep(string filePath, string word) {
	ifstream inputFile(filePath);
	if (inputFile.is_open()) {
		string line;
		while (getline(inputFile, line)) {
			if (isWordInSentence(line, word)) {
				cout << line << endl;
			}

		}
	} else {
		cout << "File was not opened" << endl;
	}
	inputFile.close();
}
