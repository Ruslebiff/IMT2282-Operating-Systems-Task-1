#include<pthread.h>
#include<semaphore.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h> 
#define N        10
#define LEFT     ((i+N-1)%N)
#define RIGHT    ((i+1)%N)
#define THINKING 0
#define HUNGRY   1
#define EATING   2
#define SHARED   0
#define NUM_ITER 100
#define TIME     1000

void *phil(void *arg);
void take_forks(int i);
void put_forks(int i);
void test(int i);

sem_t phil_s[N];        /* phil: one semaphore for each philosopher */
sem_t b,mutexprint;     /* b: binary, used as a mutex */
int state[N];           /* keep track of everyone's state */

char *text[] = {"THINK ", "HUNGRY ", "EAT "};

int main(void)
{
  int i;
  pthread_t phil_t[N];

  sem_init(&b, SHARED, 1);
  sem_init(&mutexprint, SHARED, 1);

  for(i=0;i<N;i++) {
    state[i]=THINKING;
  }

  for(i=0;i<N;i++) {
    sem_init(&phil_s[i], SHARED, 0);
    pthread_create(&phil_t[i], NULL, phil, (void*) (intptr_t) i);
  }

  for(i=0;i<N;i++) {
    pthread_join(phil_t[i], NULL);
  }

  return 0;
}

void *phil(void *arg) {
  int i=0,j;
  while(i < NUM_ITER) {
    usleep(random()%(TIME*2)); /* think 2 times longer than eating */
    take_forks((intptr_t)arg);
    printf("Filosof %ld spiser:\t",(intptr_t)arg);
    for(j=0;j<N;j++) {
      printf("%s\t",text[state[j]]);
    }
    printf("\n");
    usleep(random()%TIME);      /* eat */
    put_forks((intptr_t)arg);
    i++;    
  }
  return 0;
}

void take_forks(int i) {
  state[i]=HUNGRY;
  test(i);
}

void put_forks(int i) {
  state[i]=THINKING;
  test(LEFT);
  test(RIGHT);
}

void test(int i) {
  if(state[i]==HUNGRY && state[LEFT]!=EATING && state[RIGHT]!=EATING) {
    state[i]=EATING; 
  }
}
