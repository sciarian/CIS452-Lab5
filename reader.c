#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>


#define FOO 4096

int main(){

	//Initialize variables.	
	int shmId;
	char *shmPtr;
	key_t key;
	char quit[100]   = "quit";	
	char clear[100] = "clear";

	struct shared_buffer{
		char buffer[1024];
		int isReading;
		int isWriting;
	};
	struct shared_buffer * s_buf;

	//Generate key.
	key = ftok("server",100);

	//Generate shared 
	if ((shmId = shmget(key, FOO, IPC_CREAT | S_IRUSR | S_IWUSR))<0) {
		perror("I can't get no...\n");
		exit(1);
	}

	//Attatch to shared memory segment.
	if ((shmPtr = shmat(shmId,0,0))==(void*)-1){
		perror ("can't attach\n");
		exit(1);
	}

	//Grab reference to shared memory segment.
	s_buf = (struct shared_buffer *) shmPtr;

	//Clear screen.		
	system("clear");
	printf("\n");

	//Loop and read
	while(1){
		printf("Waiting for input...");

		//Wait fot writer
		while(s_buf->isWriting == 1);
		s_buf->isReading++;
		printf("\tread: %s\n", s_buf->buffer);

		if(strncmp(quit,s_buf->buffer,4) == 0){
			break;
		}

		if(strncmp(clear,s_buf->buffer,5) == 0){
			system("clear");
		}

		s_buf->isReading--;
	}

	//Clear screen.
	system("clear");
	printf("\nGood bye!\n");	

	//Detach
	if (shmdt(shmPtr) < 0){
		perror("Just can't let go\n");
		exit(1);
	}
}
