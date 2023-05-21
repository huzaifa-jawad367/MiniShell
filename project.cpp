// command_line_arguments.cpp  

#include <iostream>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <stdio.h>
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

int main()
{

	char *path2;
	char *arr [250];
	char *Tokenized ;
	char input[250];
	char *argv[TOKENSIZE];

	while(true)
	{
		cout<< "cwushell-> ";
		cin.getline(input,250);
		history.push_back(input);
		StrTokenizer(input, argv);  
		if (strcmp(input, "exit") == 0)  

		{
			break;
		}
		else if (strcmp(input, "\n") == 0){
			continue;
		}
		myExecvp(argv);
	}
	return 0;
}

void  myExecvp(char **argv)
{
	pid_t  pid;
	int    status;
	int childStatus;
	pid = fork();
	if(pid == 0)
	{
		if (strcmp(argv[0], "history") == 0)
		{
			PrintHistory();
			return;
		}
		
		childStatus = execvp(*argv, argv);
		if (childStatus < 0){
			cout<<"ERROR:wrong input"<<endl;
		}
		exit(0);

	}

	else if(pid < 0)
	{
		cout<< "somthing went wrong!"<<endl;
	}
	else 

	{   int status;
		waitpid(pid, &status , 0);

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