#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>

#define FOO 4096
#define rSIZE 3

int anyoneReading(int readers[2]){
	int i;
	for(i=0; i<rSIZE; i++)
		if(readers[i]==1)
			return 1;
	return 0;	
}

int main(){
	//Initialize variables.	
	int shmId;
	char *shmPtr;
	key_t key;
	char quit[100] = "quit";

	struct shared_buffer
	{
		char buffer[1024];
		int isReading[2];
		int isWriting;
		int nextReaderIndex;
	};

	struct shared_buffer s_buf_struct;
	strcpy(s_buf_struct.buffer,"");
	s_buf_struct.nextReaderIndex = 0;
	s_buf_struct.isWriting = 1;

	int i;
	for(i=0; i<rSIZE; i++){
		s_buf_struct.isReading[i] = 0;
	}

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


	memcpy(shmPtr,&s_buf_struct,sizeof(struct shared_buffer));
	struct shared_buffer * s_buf = (struct shared_buffer *) shmPtr; 

	//Loop and read
	while(1){
		while(anyoneReading(s_buf->isReading) == 1);
		
		s_buf->isWriting  = 1; 
		printf("\nEnter a message to sent to the shared memory segment.\n");
		fgets(s_buf->buffer,1024,stdin);
		
		s_buf->isWriting = 0;

		if(strncmp(s_buf->buffer,quit,4)==0){
			break;
		}
	}	

	system("clear");
	printf("\nGood bye!\n");

	//Detach	
	if (shmdt(shmPtr) < 0){
		perror("Just can't let go\n");
		exit(1);
	}

	//Deallocate (Leave this for the writer???)
	if (shmctl(shmId, IPC_RMID, 0) < 0){
		perror("can't deallocate\n");
		exit(1);
	}
}
