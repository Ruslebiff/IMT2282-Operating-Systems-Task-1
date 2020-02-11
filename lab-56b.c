#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define SHARED 0  /* process-sharing if !=0, thread-sharing if =0*/
#define BUF_SIZE 20
#define MAX_MOD 100000
#define NUM_ITER 200

void* Producer(int id); /* Producer thread */
void* Consumer(int id); /* Consumer thread */

sem_t empty;            /* empty: How many empty buffer slots */
sem_t full;             /* full: How many full buffer slots */
sem_t b;                /* b: binary, used as a mutex */

int g_data[BUF_SIZE];   /* shared finite buffer  */
int g_idx;              /* index to next available slot in buffer, 
                           remember that globals are set to zero
                           according to C standard, so no init needed  */
struct threadargs {
  int id;         /* thread number */
};


int main(int argc, char *argv[]) {
/*	main(..) parameters:
    argc (ARGument Count) is int and stores number of command-line arguments passed by the user including the name of the program. So if we pass a value to a program, value of argc would be 2 (one for argument and one for program name)
    The value of argc should be non negative.
    argv(ARGument Vector) is array of character pointers listing all the arguments.
    If argc is greater than zero,the array elements from argv[0] to argv[argc-1] will contain pointers to strings.
    Argv[0] is the name of the program , After that till argv[argc-1] every element is command -line arguments.
*/

	// Initialize variables
	long N = 0; 
	char* ptr;

	pthread_t pid, cid;
	//pthread_t *producerThread;
	//pthread_t *consumerThread;
    //int k = 1;

	// Check args
	if (argc < 2) {  // missing arguments
		printf("Missing argument for number of threads, exiting...\n");
		return 0;
	}
	if (argv[1]) { 
		N = strtol(argv[1], &ptr, 10); // TODO: test atoi(argc[1]) instead? Then (int)N can be just N
		if (N < 1) {					// argument is 0, negative or something weird
			printf("Invalid argument, exiting..\n");
			return 0;
		}
	} 

	struct threadargs *producers[N];
	struct threadargs *consumers[N];

	/* allocate memory for threadargs */
	
	for(int i=0;i<(int)N;i++) { 
		producers[i] = (struct threadargs*) malloc(sizeof(struct threadargs));
		consumers[i] = (struct threadargs*) malloc(sizeof(struct threadargs));

		producers[i]->id = i;
		consumers[i]->id = i;
	}
	//targs[0]->id=k +1; //wtf


	// Initialie the semaphores
	sem_init(&empty, SHARED, BUF_SIZE);
	sem_init(&full, SHARED, 0);
	sem_init(&b, SHARED, 1);

	// Create the threads
	printf("main started\n");
	printf("Creating %d producers and consumers\n", (int)N);
	for (int i = 0; i < (int)N; i++) {
		pthread_create(&pid, NULL, Producer, producers[i]->id);
		pthread_create(&cid, NULL, Consumer, consumers[i]->id);
	}
	

	// And wait for them to finish.
	for (int i = 0; i < (int)N; i++) {
		pthread_join(pid, NULL);
		pthread_join(cid, NULL);
	}
	
	printf("main done\n");

	return 0;
}


void *Producer(int id) {
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
		j=0; printf("(Producer %d, idx is %d): ", id, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exlusive access
		sem_post(&b);
		// Increase the counter of full bufferslots.
		sem_post(&full);
		
		i++;		
	}

	return 0;
}


void *Consumer(int id) {
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
		j=0; printf("(Consumer %d, idx is %d): ", id, g_idx);
		while(j < g_idx) { j++; printf("="); } printf("\n");
		
		// Leave region with exclusive access
		sem_post(&b);
		// Increase the counter of empty slots.
		sem_post(&empty);  	

		i++;
	}

	return 0;

}

