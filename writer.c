#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define FOO 4096
#define rSIZE 3

int main(){
	//Initialize variables.	
	int shmId;
	char *shmPtr;
	key_t key;
	char clear[100] = "clear";
	char quit[100]  = "quit";

	struct shared_buffer{
		char buffer[1024];
		int isReading;
		int isWriting;
	};

	struct shared_buffer s_buf_struct;
	strcpy(s_buf_struct.buffer,"");
	s_buf_struct.isReading = 0;
	s_buf_struct.isWriting = 1;

	//Generate key.
	if((key = ftok("server",100))>0){
		perror("Failed to create key.");
		exit(1);
	}

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

	//Copy shared buffer structure into shared memory.
	memcpy(shmPtr,&s_buf_struct,sizeof(struct shared_buffer));

	//Grab a pointer to the shared memory.
	struct shared_buffer * s_buf = (struct shared_buffer *) shmPtr; 

	system("clear");

	//Loop and read.
	while(1){

		//Wait for readers
		while(s_buf->isReading > 0);
		
		s_buf->isWriting  = 1; 
		printf("\nEnter a message to sent to the shared memory segment.\n");
		fgets(s_buf->buffer,1024,stdin);
		
		s_buf->isWriting = 0;

		if(strncmp(s_buf->buffer,quit,4)==0){
			break;
		}

		if(strncmp(s_buf->buffer,clear,5)==0){
			system("clear");
		}
	}	

	//Clear screen.
	system("clear");
	printf("\nGood bye!\n");

	//Detach	
	if (shmdt(shmPtr) < 0){
		perror("Just can't let go\n");
		exit(1);
	}

	//Deallocate 
	if (shmctl(shmId, IPC_RMID, 0) < 0){
		perror("can't deallocate\n");
		exit(1);
	}
}
