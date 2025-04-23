#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define NUM_SAVAGES 5
#define POT_CAPACITY 3
#define GRID_WIDTH 50
#define GRID_HEIGHT 31
#define REFRESH_RATE 0.5  // Faster refresh rate

typedef enum {
    IN_TRIBE,
    GOING_TO_POT,
    EATING,
    RETURNING,
    FULL
} SavageState;

typedef struct {
    SavageState state;
    int x, y;
    int dest_x, dest_y;
    pthread_mutex_t mutex;
} SavageStatus;

typedef struct {
    int x, y;
    int dest_x, dest_y;
    int is_sleeping;
    pthread_mutex_t mutex;
} CookStatus;

SavageStatus savages[NUM_SAVAGES];
CookStatus cook_status = {10, 25, 10, 25, 1, PTHREAD_MUTEX_INITIALIZER};
int servings = POT_CAPACITY;
const int pot_x = 10, pot_y = 15;

sem_t pot_mutex;
sem_t pot_empty;
sem_t eating_mutex;
sem_t pot_refilled;
sem_t pot_access;

void clear_screen() {
    printf("\033[2J\033[H");
}

void init_positions() {
    for (int i = 0; i < NUM_SAVAGES; i++) {
        savages[i].x = 2;
        savages[i].y = 2 + i * 4;
        savages[i].dest_x = savages[i].x;
        savages[i].dest_y = savages[i].y;
        savages[i].state = FULL;
        pthread_mutex_init(&savages[i].mutex, NULL);
    }
}

void update_position(int id) {
    pthread_mutex_lock(&savages[id].mutex);
    if (savages[id].x < savages[id].dest_x) savages[id].x++;
    else if (savages[id].x > savages[id].dest_x) savages[id].x--;
    if (savages[id].y < savages[id].dest_y) savages[id].y++;
    else if (savages[id].y > savages[id].dest_y) savages[id].y--;
    pthread_mutex_unlock(&savages[id].mutex);
}

void update_cook_position() {
    pthread_mutex_lock(&cook_status.mutex);
    if (cook_status.x < cook_status.dest_x) cook_status.x++;
    else if (cook_status.x > cook_status.dest_x) cook_status.x--;
    if (cook_status.y < cook_status.dest_y) cook_status.y++;
    else if (cook_status.y > cook_status.dest_y) cook_status.y--;
    pthread_mutex_unlock(&cook_status.mutex);
}

void print_grid() {
    clear_screen();
    char grid[GRID_HEIGHT][GRID_WIDTH];
    memset(grid, ' ', sizeof(grid));

    for (int i = 0; i < GRID_HEIGHT; i++) {
        grid[i][0] = '#';
        grid[i][GRID_WIDTH-1] = '#';
    }
    for (int i = 0; i < GRID_WIDTH; i++) {
        grid[0][i] = '#';
        grid[GRID_HEIGHT-1][i] = '#';
    }

    grid[pot_x][pot_y] = 'P';

    pthread_mutex_lock(&cook_status.mutex);
    grid[cook_status.x][cook_status.y] = 'C';
    if (cook_status.is_sleeping) {
        grid[cook_status.x-1][cook_status.y] = 'z';
        grid[cook_status.x-2][cook_status.y] = 'z';
        grid[cook_status.x-3][cook_status.y] = 'z';
    }
    pthread_mutex_unlock(&cook_status.mutex);

    for (int i = 0; i < NUM_SAVAGES; i++) {
        pthread_mutex_lock(&savages[i].mutex);
        char c = '1' + i;
        grid[savages[i].x][savages[i].y] = c;
        if (savages[i].state == FULL) {
            grid[savages[i].x+1][savages[i].y] = 'x';
        }
        pthread_mutex_unlock(&savages[i].mutex);
    }
    for (int i = 0; i < GRID_HEIGHT; i++) {
        for (int j = 0; j < GRID_WIDTH; j++) {
            putchar(grid[i][j]);
        }
        putchar('\n');
    }

    printf("\nServings: %d | Cook: %s\n", servings, cook_status.is_sleeping ? "SLEEPING" : "AWAKE");
    fflush(stdout);
}

void *status_printer(void *arg) {
    while (1) {
        for (int i = 0; i < NUM_SAVAGES; i++) {
            update_position(i);
        }
        update_cook_position();
        print_grid();
        usleep(REFRESH_RATE * 1000000);
    }
    return NULL;
}

void *savage(void *arg) {
    int id = *(int *)arg;
    struct timespec ts;

    while (1) {
        pthread_mutex_lock(&savages[id].mutex);
        if (savages[id].state == FULL) {
            savages[id].state = IN_TRIBE;
            pthread_mutex_unlock(&savages[id].mutex);
            usleep((rand() % 500000) + 100000);
        } else {
            pthread_mutex_unlock(&savages[id].mutex);
            usleep(100000);
            continue;
        }

        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        while (sem_timedwait(&pot_access, &ts) == -1) {
            pthread_mutex_lock(&savages[id].mutex);
            if (savages[id].state != IN_TRIBE) {
                pthread_mutex_unlock(&savages[id].mutex);
                goto end_cycle;
            }
            pthread_mutex_unlock(&savages[id].mutex);
            clock_gettime(CLOCK_REALTIME, &ts);
            ts.tv_sec += 1;
        }

        pthread_mutex_lock(&savages[id].mutex);
        savages[id].state = GOING_TO_POT;
        savages[id].dest_x = pot_x - 1;
        savages[id].dest_y = pot_y;
        pthread_mutex_unlock(&savages[id].mutex);

        while (1) {
            pthread_mutex_lock(&savages[id].mutex);
            if (savages[id].x == pot_x - 1 && savages[id].y == pot_y) break;
            pthread_mutex_unlock(&savages[id].mutex);
            usleep(100000);
        }
        pthread_mutex_unlock(&savages[id].mutex);

        sem_wait(&eating_mutex);
        sem_wait(&pot_mutex);
        if (servings == 0) {
            sem_post(&pot_empty);
            sem_post(&pot_mutex);
            sem_wait(&pot_refilled);
            sem_wait(&pot_mutex);
        }
        servings--;
        sem_post(&pot_mutex);

        pthread_mutex_lock(&savages[id].mutex);
        savages[id].state = EATING;
        pthread_mutex_unlock(&savages[id].mutex);

        sem_post(&eating_mutex);

        pthread_mutex_lock(&savages[id].mutex);
        savages[id].state = RETURNING;
        savages[id].dest_x = 2;
        savages[id].dest_y = 2 + id * 4;
        pthread_mutex_unlock(&savages[id].mutex);

        while (1) {
            pthread_mutex_lock(&savages[id].mutex);
            if (savages[id].x == 2 && savages[id].y == (2 + id * 4)) break;
            pthread_mutex_unlock(&savages[id].mutex);
            usleep(100000);
        }

        savages[id].state = FULL;
        pthread_mutex_unlock(&savages[id].mutex);
        sem_post(&pot_access);

        end_cycle:
        usleep(2000000);
    }
    return NULL;
}

void *cook_thread(void *arg) {
    while (1) {
        sem_wait(&pot_empty);

        pthread_mutex_lock(&cook_status.mutex);
        cook_status.is_sleeping = 0;
        cook_status.dest_x = pot_x;
        cook_status.dest_y = pot_y;
        pthread_mutex_unlock(&cook_status.mutex);

        while (1) {
            pthread_mutex_lock(&cook_status.mutex);
            if (cook_status.x == pot_x && cook_status.y == pot_y) break;
            pthread_mutex_unlock(&cook_status.mutex);
            usleep(100000);
        }
        pthread_mutex_unlock(&cook_status.mutex);
        usleep(1000000);
        sem_wait(&pot_mutex);
        servings = POT_CAPACITY;
        sem_post(&pot_mutex);
        sem_post(&pot_refilled);

        pthread_mutex_lock(&cook_status.mutex);
        cook_status.dest_x = 10;
        cook_status.dest_y = 25;
        pthread_mutex_unlock(&cook_status.mutex);

        while (1) {
            pthread_mutex_lock(&cook_status.mutex);
            if (cook_status.x == 10 && cook_status.y == 25) break;
            pthread_mutex_unlock(&cook_status.mutex);
            usleep(100000);
        }

        cook_status.is_sleeping = 1;
        pthread_mutex_unlock(&cook_status.mutex);
    }
    return NULL;
}

int main() {
    pthread_t savage_threads[NUM_SAVAGES];
    pthread_t cook_thr, status_thread;
    int ids[NUM_SAVAGES];
    srand(time(NULL));

    init_positions();
    sem_init(&pot_mutex, 0, 1);
    sem_init(&pot_empty, 0, 0);
    sem_init(&eating_mutex, 0, 1);
    sem_init(&pot_refilled, 0, 0);
    sem_init(&pot_access, 0, 1);

    pthread_create(&status_thread, NULL, status_printer, NULL);
    pthread_create(&cook_thr, NULL, cook_thread, NULL);

    for (int i = 0; i < NUM_SAVAGES; i++) {
        ids[i] = i;
        pthread_create(&savage_threads[i], NULL, savage, &ids[i]);
    }

    for (int i = 0; i < NUM_SAVAGES; i++) {
        pthread_join(savage_threads[i], NULL);
    }

    pthread_join(cook_thr, NULL);
    pthread_join(status_thread, NULL);

    for (int i = 0; i < NUM_SAVAGES; i++) {
        pthread_mutex_destroy(&savages[i].mutex);
    }
    pthread_mutex_destroy(&cook_status.mutex);
    sem_destroy(&pot_mutex);
    sem_destroy(&pot_empty);
    sem_destroy(&eating_mutex);
    sem_destroy(&pot_refilled);
    sem_destroy(&pot_access);

    return 0;
}
