#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>

#define FOO 4096

int main(){

	//Initialize variables.	
	int shmId;
	char *shmPtr;
	key_t key;
	char quit[100] = "quit";	
	char buffer[1024];
	int index;

	struct shared_buffer{
		char buffer[1024];
		int isReading[2];
		int isWriting;
		int nextReaderIndex;
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

	s_buf = (struct shared_buffer *) shmPtr;
	index = s_buf->nextReaderIndex;
	s_buf->nextReaderIndex += 1;

	printf("\nReady to read, my index is: %d\n",index);

	//Loop and read
	while(1){
		printf("Waiting for input...\n");
		sleep(0.250);
		while(s_buf->isWriting == 1);
		s_buf->isReading[index] = 1;
		strcpy(buffer, s_buf->buffer);	
		printf("\nRead: %s", buffer);

		if(strncmp(quit,buffer,4) == 0){
			break;
		}

		s_buf->isReading[index]  = 0;

	}

	system("clear");
	printf("\nGood bye!\n");	

	//Detach
	if (shmdt(shmPtr) < 0){
		perror("Just can't let go\n");
		exit(1);
	}


	//Deallocate (Leave this for the writer???)
	/*
	   if (shmctl(shmId, IPC_RMID, 0) < 0){
	   perror("can't deallocate\n");
	   exit(1);
	   }
	   */
}
