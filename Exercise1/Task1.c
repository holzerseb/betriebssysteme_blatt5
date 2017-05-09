#include <time.h>
#include "Task1.h"
#include "loggerService.h"

/* Creates a Pipe with the given name
   and opens it for writing */
FILE *createPipe(char* pipeName);
/* Opens a Pipe for reading */
FILE *openPipe(char* pipeName);

int main(void)
{
	printf("[INFO] Starting Program...\n");
	//We start by initializing the names for our server-pipes
	char *dbServerPipe = "DBServer-Pipe";
	char *mwServerPipe = "MWServer-Pipe";
	char *webServerPipe = "WebServer-Pipe";
	srand(time(NULL));

/*
			printf("[INFO] creating...\n");
	FILE *test = createPipe("Test");
			printf("[INFO] created...\n");
	addPipeToLogger(test);
			printf("[INFO] finish...\n");
	exit(0);
*/

	printf("[INFO] Starting Servers...\n");
	//we create 3 processes to "simulate" each server
	for(int i=2; i > -1; i--)
	{
		int pid = fork();

		if(pid < 0) //I'm not doing error-handling here
		{
			fprintf(stderr, "Couldn't fork\n");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			printf("[INFO] Child-Process created...\n");
			//this is now one of the servers
			FILE *fp_serverPipe;
			char *serverPipeName;
			switch(i)	//there might have been a better solution
			{		//but hell does it work :^}
				case 2:
					serverPipeName = dbServerPipe;
					break;
				case 1:
					serverPipeName = mwServerPipe;
					break;
				case 0:
				default:
					serverPipeName = webServerPipe;
					break;
			}

			//we create the pipe of this server and open it for writing
			fp_serverPipe = createPipe(serverPipeName);
			printf("[INFO] Pipe succesfully created...\n");

			//also, we add this pipe directly to our logger
			addPipeToLogger(fp_serverPipe);
			printf("[INFO] Pipe added to logger...\n");

			//so, since each child has it's own currentPipe now,
			//we can start the run "forever"
			printf("[INFO] Server #%d now runs forever...\n", i);
			while(1)
			{
				//we gonna sleep for rand(2,7) seconds
				sleep((rand() % 6) + 2);
				
				//then we gonna log something
				fprintf(fp_serverPipe, "This is a very original Message from Server \"%s\"\n", serverPipeName);
				//^^ excuse me for not resolving the serverpipename to the servername...
			}
			
			//don't forget to exit the child here
			exit(EXIT_SUCCESS);
		}
	}

	printf("[INFO] Logger will now start...\n");
	//And start the logger
	startLogger(); //causes the current process to stay in endlessloop, tracking for log-messages

	//And usually you wouldn't even come this far
	exit(EXIT_SUCCESS);
}

FILE *createPipe(char* pipeName)
{	
	//first we create the path where we store our fifos
	char *basePath = "pipes";
	printf("[INFO] Check if PIPE-Directory exists...\n");
	mkdir(basePath, 0777); //yes, I'm not considering security here
		//also, we just ensure here the directory exists
	
	//Next, we create the full path of the pipe
	char pipePath[(strlen(basePath) + strlen(pipeName) + 2)];
	strcpy(pipePath, basePath);
	strcat(pipePath, "/");
	strcat(pipePath, pipeName);

	//then we create the pipe
	printf("[INFO] Create PIPE-File now...\n");
	if(mkfifo(pipePath, 0777) == -1)
	{
		fprintf(stderr, "Couldn't create the FIFO at \"%s\"\n", pipePath);
		exit(EXIT_FAILURE);
	}

	//connect to it
	FILE *fp;
	printf("[INFO] Open Pipe-File \"%s\" for writing...\n", pipePath);
	if( (fp = fopen(pipePath, "r")) == NULL)
	{
		printf("Couldn't open the FIFO at \"%s\"\n", pipePath);
		fprintf(stderr, "Couldn't open the FIFO at \"%s\"\n", pipePath);
		exit(EXIT_FAILURE);
	}

	//and return this File
	return fp;
}
