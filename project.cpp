// command_line_arguments.cpp  

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
#include <fstream>

#include <sys/types.h>
#include <sys/wait.h>


#define TOKENSIZE 100


using namespace std;
void  StrTokenizer(char *line, char **argv);
void  myExecvp(char **argv);
int GetEnv();


int main() {
    char *path2;
    char *arr[250];
    char *Tokenized;
    char input[250];
    char *argv[TOKENSIZE];

    while (true) {
        cout << "cwushell-> ";
        cin.getline(input, 250);
        StrTokenizer(input, argv);
        if (strcmp(argv[0], "exit") == 0) {
            break;
        } else if (strcmp(argv[0], ">>") == 0) {
            myExecvp(argv);
        } else if (strcmp(input, "\n") == 0) {
            continue;
        } else {
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
        char* outFile;
        
        // Check for '>>' operator
        for (int i = 0; argv[i] != NULL; i++) {
            if (strcmp(argv[i], ">>") == 0) { //to allow cat >> command
                appendMode = true;
                outFile = argv[i + 1];
                argv[i] = NULL;
                break;
            }
        }

        if (appendMode) {
            std::fstream file(outFile, std::ios::app); // Open file in append mode
            std::string input;

            // Read user input until termination string is entered
            while (true) {
                std::getline(std::cin, input); 
                if (std::cin.eof()) { // allows user to enter ctrl + d to end user input 
                    break;
                }
                file << input << std::endl;
            }
            file.close();
            exit(0);
        } else {
            childStatus = execvp(*argv, argv);
            if (childStatus < 0) {
                cout << "ERROR: wrong input" << endl;
            }
            exit(0);
        }
    } else if (pid < 0) {
        cout << "something went wrong!" << endl;
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

void StrTokenizer(char *input, char **argv)
{
	char *stringTokenized;
	stringTokenized = strtok(input, " ");
	while(stringTokenized != NULL)
	{
		*argv++  = stringTokenized;
		stringTokenized = strtok(NULL, " ");
	}

	*argv = NULL;
}

int GetEnv()
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
