#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
	//Init
	int pipefd[2]; //file-descriptor array for the pipe
	pid_t firstProcessPid, secondProcessPid;

	//first we read the keyword
	char keyword[100]; //who would search for more then 100 chars after a "ls"
	printf("Enter the Keyword: ");
	if(scanf("%[^\n]%*c", keyword) < 0) //[] is scanset, ^\n tells "while input is not newline"
	{
		perror("Pipe-Creation failed\n");
		exit(EXIT_FAILURE);
	}

	//We create a pipe then
	if(pipe(pipefd) == -1)
	{
		perror("Pipe-Creation failed\n");
		exit(EXIT_FAILURE);
	}

	//then create a child-processes to perform "ls"
	if( (firstProcessPid = fork()) == -1)
	{
		perror("First Fork failed\n");
		exit(EXIT_FAILURE);
	}

	if(firstProcessPid == 0)
	{ //this will perform ls

		//This process wants to send data, so we close fd[0]
		close(pipefd[0]); //pipefd[0] is the reading end

		//next, we redirect the output of this process to the pipe
		dup2(pipefd[1], 1); //1 == stdout
		
		//actually, pipefd[1] is no longer needed in this process
		close(pipefd[1]);

		//perform the "ls" command
		char *args[2]; //required for a proper execv call
		args[0] = "/bin/ls"; //args include the path as first arg by convention
		args[1] = NULL; //args must be NULL terminated
		execv(args[0], args); //perform command call
		exit(EXIT_SUCCESS);
	}
	
	//next, we create the second process to perform "grep <keyword>"
	if( (secondProcessPid = fork()) == -1)
	{
		perror("Second Fork failed\n");
		exit(EXIT_FAILURE);
	}

	//The parent now has to create another process
	if(secondProcessPid == 0)
	{ //this will perform the grep part

		//This process wants to receive data, so we close the writing end
		close(pipefd[1]);

		
		//we want grep to read from the read-end
		dup2(pipefd[0], 0);
		close(pipefd[0]); //not required anymore
		
		//next, perform the grep command on the ls-return-value
		char *args[3]; //required for a proper execv call
		args[0] = "/bin/grep"; //args include the path as first arg by convention
		args[1] = keyword;
		args[2] = NULL; //args must be NULL terminated
		execv(args[0], args); //perform command call
	}

	exit(EXIT_SUCCESS);
}
