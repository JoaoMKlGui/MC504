#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#define SAVAGES 5
#define MAXPOT 10

int pot = 0;

pthread_mutex_t mutex_pot = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t empty_pot = PTHREAD_COND_INITIALIZER;

void putServingsInPot() {
    printf("Cozinheiro está enchendo o pote!\n");
    pot = MAXPOT;
}

void* cook(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex_pot);
        while (pot > 0) {
            pthread_cond_wait(&empty_pot, &mutex_pot);
        }
        putServingsInPot();
        pthread_mutex_unlock(&mutex_pot);
    }
    return NULL;
}

void getServingFromPot() {
    pot--;
    printf("Um selvagem pegou comida. Restam %d porções.\n", pot);
}

void eat() {
    printf("Selvagem está comendo...\n");
    sleep(1);
}

void* dining(void* arg) {
    while (1) {
        pthread_mutex_lock(&mutex_pot);
        if (pot == 0) {
            pthread_cond_signal(&empty_pot);
        }
        while (pot == 0) {
            //espera o cozinheiro encher o pote
            pthread_mutex_unlock(&mutex_pot);
            usleep(100000); //pequena espera antes de tentar de novo
            pthread_mutex_lock(&mutex_pot);
        }

        getServingFromPot();
        pthread_mutex_unlock(&mutex_pot);

        eat();
    }
    return NULL;
}

int main() {
    pthread_t savages[SAVAGES];
    pthread_t chef;

    pthread_create(&chef, NULL, cook, NULL);

    for (int i = 0; i < SAVAGES; i++) {
        pthread_create(&savages[i], NULL, dining, NULL);
    }

    pthread_join(chef, NULL); //essa linha nunca retorna,já que o cozinheiro roda pra sempre

    for (int i = 0; i < SAVAGES; i++) {
        pthread_join(savages[i], NULL); 
    }

    return 0;
}
