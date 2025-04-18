#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>

pthread_mutex_t mutex_pot = PTHREAD_MUTEX_INITIALIZER;

#define SAVAGES 5

const int MAXPOT = 10;
int pot = MAXPOT;

void putServingsInPot() {
  pot = MAXPOT;
}

void cook() {
  while (1) {
    pthread_mutex_lock(&mutex_pot); // mutex lock para acessar e entrar na regiao critica

    if (pot == 0) 
      putServingsInPot();      

    pthread_mutex_unlock(&mutex_pot); // unlock mutex
  }
}

void getServingFromPot() {
    pot--;
}

void eat() {
  // faz nada
}

void dining() {
  while (1) {

    pthread_mutex_lock(&mutex_pot); // lock mutex para acessar a regiao critica de servir -> similar a vez dele na fila 
    if (pot > 0) {
      getServingFromPot();
      eat();
    }

    pthread_mutex_unlock(&mutex_pot) // unlock mutex -> ja se serviu e comeu
  }
}

int main() {

  pthread_t *thread_group = malloc(sizeof(pthread_t) * SAVAGES);
  pthread_t *chef;  

  for (int i = 0; i < SAVAGES; i++) {
    pthread_create(&thread_group[i], NULL, dining, NULL);
  }
  
  pthread_create(&chef, NULL, cook, NULL);
  pthread_join(chef);
  
  for (int i = 0; i < SAVAGES; i++) {
    pthread_create(thread_group[i], NULL);
  }

  return 0;
}