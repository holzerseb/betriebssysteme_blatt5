#include "loggerService.h"
#include <sys/select.h>
#include <stdio.h>

static FILE **pipesToListen;
static int pipeCounter = 0;
void Validate(void);

void addPipeToLogger(FILE *pipe)
{
	pipeCounter++;
	pipesToListen = realloc(pipesToListen, sizeof(FILE*) * pipeCounter);
	pipesToListen[pipeCounter-1] = pipe;
}

void startLogger(void)
{
	//This validates the logger
	Validate();

	//first, we have to create a set of file-descriptors, which we gonna listen
	fd_set fds;
	int maxfd = 0; //Descriptors are just ints, here we save the highest-numbered descriptor
	int count; //for scanf
	char buf[256]; //buffer for scanf()

	printf("[LOGGER] Logger will listen to %d Pipes...\n", pipeCounter);
	FD_ZERO(&fds); // Clear FD set for select
	for(int i = pipeCounter-1; i >= 0; i--)
	{
		//we have to convert our file-Pointer to a file-descriptor here
		FD_SET(fileno(pipesToListen[i]), &fds);
		if(fileno(pipesToListen[i]) > maxfd)
			maxfd = fileno(pipesToListen[i]);
	}

	//then we gonna listen on the pipes
	//task was to create endless lop...
	while(1)
	{
		//because we are only interested in reading, we feed the fds into the readfds-param
		select(maxfd + 1, &fds, NULL, NULL, NULL);

		//and because we want to know which pipe was triggered:
		for(int i = pipeCounter-1; i >= 0; i--)
		{
			if(FD_ISSET(fileno(pipesToListen[i]), &fds))
			{
				// There is data available to be read
				while((count = fscanf(pipesToListen[i], "%s", buf)) > 0)
				{
					printf("%s", buf);
				}
			}
		}
		
	}

	//when the logger has finished, we free up the memory
	free(pipesToListen);
}

void Validate()
{
	//I am reaaally sorry for this
	//I would have usually send a signal from each Server to the parent
	//and let the parent wait for all the signals so he knows the servers
	//are ready, before he would start the logger
	//but it's 2am and this exercise was about pipes :)
	printf("[LOGGER] Waiting for all Servers to be running and their pipes be ready...\n");
	while(pipeCounter < 3)
	{
		//as long as there aint all pipes, sleep
		sleep(2);
	}
}

