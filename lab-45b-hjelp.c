#include <stdio.h>  /* printf */
#include <stdlib.h> /* exit */
#include <sys/wait.h> /* waitpid */
#include <unistd.h> /* fork */

void process(int number, int time) {
  printf("Prosess %d kjører\n", number);
  sleep(time);
  printf("Prosess %d kjørte i %d sekunder\n", number, time);
}

int main() {
  int p[6];

  // START P0
  // Fork en ny proc
  p[0] = fork(); 
  // Hvis barn:
  if(p[0] == 0) {
    // Kjør proc func
    process(0, 1);
    // Avslutt child
    exit(0); 
  }

  // START P2
  // Fork en ny proc
  p[2] = fork(); 
  // Hvis barn:
  if(p[2] == 0) {
    // Kjør proc func
    process(2, 3);
    // Avslutt child
    exit(0); 
  }

  // VENT P0
  waitpid(p[0], NULL, 0);

  // START P1
  // VENT P1
  // VENT P2
  // START P3
  // VENT P4
  // START P5
  // VENT P3
  // VENT P5

  return 0;
}
