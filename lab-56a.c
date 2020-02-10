#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>     /* printf */
#include <stdlib.h>    /* exit */
#include <unistd.h>    /* fork */
#define SHARED 1

sem_t sem[6];     /* one semaphore for each thread */

struct threadargs {
  int id;         /* thread number */
  int sec;        /* how many seconds to sleep */
  int signal[6];  /* which threads to signal when done */
};


/* thread function: start by waiting on my own semaphore if it has been
 * initialized to 0; do my work (sleeping); signal the threads that
 * should start when I finish; exit
*/
void* tfunc(void *arg) {
  int i;
  struct threadargs *targs=arg;
  sem_wait(&sem[targs->id-1]);
  printf("Tråd %d kjører\n", targs->id);
  sleep(targs->sec);
  printf("Tråd %d er ferdig og vekker kanskje andre...\n", targs->id);
  for(i=0;i<6;i++) {
    if(targs->signal[i]==1) { sem_post(&sem[i]); }
  }
  pthread_exit(NULL);
}


int main(void)
{
  int i,j;
  pthread_t tid[6];
  struct threadargs *targs[6];

  /* allocate memory for threadargs and zero out semaphore signals */
  for(i=0;i<6;i++) { 
    targs[i] = (struct threadargs*) malloc(sizeof(struct threadargs));
    for(j=0;j<6;j++) { targs[i]->signal[j]=0; }
  }

  targs[0]->id=1;             /* thread number 1 */
  targs[0]->sec=1;            /* how long to sleep */
  targs[0]->signal[1]=1;      /* which threads to wake up when done */
  targs[0]->signal[4]=1;
  sem_init(&sem[targs[0]->id-1],SHARED,1); /* start waiting or running? */
  pthread_create(&tid[0], NULL, tfunc, (void *) targs[0]);

// GJENTA KODELINJENE OVER FOR DE FEM NESTE TRÅDENE, BARE ENDRE PARAMETRE

  for(i=0;i<1;i++) {
    pthread_join(tid[i], NULL);
  }

  return 0;
}









