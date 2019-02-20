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
	char quit[100] = "quit";	

	struct shared_buffer{
		char buffer[1024];
		int isReading;
		int isWriting;
	};
	struct shared_buffer s_buf;

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

	
	//Loop and read
	while(1){
		memcpy(&s_buf,shmPtr,sizeof(struct shared_buffer));
		//if it is out turn to read read TODO
		if(s_buf.isReading == 1){
			
			printf("\nRead: %s\n", s_buf.buffer);

			if(strncmp(quit,shmPtr,4) == 0){
				break;
			}

			s_buf.isReading  = 0;
			s_buf.isWriting = 1;
	
			memcpy(shmPtr,&s_buf,sizeof(struct shared_buffer));
		}
		 
	}	

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
