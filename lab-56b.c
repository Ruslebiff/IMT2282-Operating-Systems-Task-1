#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHARED 0  /* process-sharing if !=0, thread-sharing if =0*/
#define BUF_SIZE 20
#define MAX_MOD 100000
#define NUM_ITER 200

void* Producer(); /* Producer thread */
void* Consumer(); /* Consumer thread */

sem_t empty;            /* empty: How many empty buffer slots */
sem_t full;             /* full: How many full buffer slots */
sem_t b;                /* b: binary, used as a mutex */

int g_data[BUF_SIZE];   /* shared finite buffer  */
int g_idx;              /* index to next available slot in buffer, 
                           remember that globals are set to zero
                           according to C standard, so no init needed  */
struct threadargs {
  int id;         /* thread number */
  int sec;        /* how many seconds to sleep */
  int signal[6];  /* which threads to signal when done */
};


int main(void) {
	pthread_t pid, cid;
    int k = 1;

  struct threadargs *targs[6];

  /* allocate memory for threadargs and zero out semaphore signals */
  for(int i=0;i<6;i++) { 
    targs[i] = (struct threadargs*) malloc(sizeof(struct threadargs));
    for(int j=0;j<6;j++) { targs[i]->signal[j]=0; }
  }

 targs[0]->id=k +1;

	// Initialie the semaphores
	sem_init(&empty, SHARED, BUF_SIZE);
	sem_init(&full, SHARED, 0);
	sem_init(&b, SHARED, 1);

	// Create the threads
	printf("main started\n");
	pthread_create(&pid, NULL, Producer, NULL);
	pthread_create(&cid, NULL, Consumer, NULL);

	// And wait for them to finish.
	pthread_join(pid, NULL);
	pthread_join(cid, NULL);
	printf("main done\n");

	return 0;
}


void *Producer() {
	int i=0, j;

	while(i < NUM_ITER) {
		// pretend to generate an item by a random wait
		usleep(random()%MAX_MOD);
		
		// Wait for at least one empty slot
		sem_wait(&empty);
		// Wait for exclusive access to the buffer
		sem_wait(&b);
		
		// Check if there is content there already. If so, print 
    // a warning and exit.
		if(g_data[g_idx] == 1) { 
			printf("Producer overwrites!, idx er %d\n",g_idx); 
			exit(0); 
		}
		
		// Fill buffer with "data" (ie: 1) and increase the index.
		g_data[g_idx]=1;
		g_idx++;
		
		// Print buffer status.
		j=0; printf("(Producer, idx is %d): ",g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exlusive access
		sem_post(&b);
		// Increase the counter of full bufferslots.
		sem_post(&full);
		
		i++;		
	}

	return 0;
}


void *Consumer() {
	int i=0, j;

	while(i < NUM_ITER) {
		// Wait a random amount of time, simulating consuming of an item.
		usleep(random()%MAX_MOD);
		
		// Wait for at least one slot to be full
		sem_wait(&full);
		// Wait for exclusive access to the buffers
		sem_wait(&b);
		
		// Checkt that the buffer actually contains some data 
    // at the current slot.
		if(g_data[g_idx-1] == 0) { 
			printf("Consumes nothing!, idx er %d\n",g_idx);
			exit(0);
		}
		
		// Remove the data from the buffer (ie: Set it to 0) 
		g_data[g_idx-1]=0;
		g_idx--;
		
		// Print the current buffer status
		j=0; printf("(Consumer, idx is %d): ",g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exclusive access
		sem_post(&b);
		// Increase the counter of empty slots.
		sem_post(&empty);  	

		i++;
	}

	return 0;

}

