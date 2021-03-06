#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "pthread_barrier.h"

#define ROW 4096
#define COL 4096
#define N_THREADS 2
#define WORK_LOAD (ROW/N_THREADS)

typedef struct Arg {
    int starting_row;
    pthread_barrier_t* barrier;
    bool* converged;
    bool* all_converged;
    int thread_id;
    int iterations;
} Arg;
    
/*Globals*/
static float plate[ROW][COL];
static float copy[ROW][COL];

void printPlate() {
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%.1f ", plate[i][j]);
        }
        printf("\n");
    }
}

void initializePlate(float plate[ROW][COL]) {
    
    //initialize top row (statically initialized to zero)

    //initialize bottom row
    for (int j = 0; j < COL; j++) {
        plate[ROW-1][j] = 100;
    }
    
    //initialize main body
    for (int i = 1; i < ROW-1; i++) {
        for (int j = 1; j < COL-1; j++) {
            plate[i][j] = 50;
        }
    }
    
    //initialize row 400
    for (int j = 0; j < 331; j++) {
        plate[400][j] = 100;
    }

    //initialize (200,500)
    plate[200][500] = 100;
    
}

void* thread_simulate(void* arg) {
    Arg* param = (Arg*) arg;
    pthread_barrier_t* barrier = (pthread_barrier_t*) param->barrier;
    int starting_row = param->starting_row;
    int thread_id = param->thread_id;
    bool* all_converged = param->all_converged;
    int iterations = 0;

    do {
        memcpy(&copy[starting_row][0], &plate[starting_row][0], 
               WORK_LOAD*COL*sizeof(float));
        for (int i = starting_row; 
             i < starting_row+WORK_LOAD && i < ROW-1; 
             i++) {
            for (int j = 1; j < COL-1; j++) {
                if (i == 400 && j <= 330) {
                    j = 330;
                    continue;
                } else if (i == 200 && j == 500) {
                    continue;
                } else if (i == 0) {
                    j = COL-1;
                    continue;
                } else {
                    float north = copy[i-1][j];
                    float west = copy[i][j-1];
                    float east = copy[i][j+1];
                    float south = copy[i+1][j];
                    float present = 4 * copy[i][j];
                    plate[i][j] = (north + west + east + south + present) / 8;
                }
            }
        }

        pthread_barrier_wait(barrier);

        for (int i = starting_row; 
             i < starting_row+WORK_LOAD && i < ROW-1; 
             i++) {
            for (int j = 1; j < COL-1; j++) {
                if (i == 400 && j <= 330) {
                    j = 330;
                    continue;
                } else if (i == 200 && j == 500) {
                    continue;
                } else if (i == 0) {
                    j = COL-1;
                    continue;
                } else {
                    float north = plate[i-1][j];
                    float west = plate[i][j-1];
                    float east = plate[i][j+1];
                    float south = plate[i+1][j];
                    float average = (north + west + east + south) / 4;
                    if (fabsf(plate[i][j] - average) >= 0.1) {
                        param->converged[thread_id] = false;
                        goto breakout;
                    }
                }
            }
        }
        param->converged[thread_id] = true;
        breakout:
        pthread_barrier_wait(barrier);
        if (thread_id == 0) {
            bool converged_accum = true;
            for (int i = 0; i < N_THREADS; i++) {
                converged_accum = converged_accum && param->converged[i];
            }            
            iterations++;
            if (converged_accum == true) {
                param->iterations = iterations;
                *all_converged = converged_accum;
            } 
        }
        pthread_barrier_wait(barrier);
    } while (!(*all_converged));
    return NULL;
}

int main(int argc, char** argv) {
    initializePlate(plate);
    pthread_t threads[N_THREADS];
    Arg args[N_THREADS];
    static bool converged[N_THREADS];
    pthread_barrier_t barrier;
    bool all_converged = false;
    int iterations = 0;

    pthread_barrier_init(&barrier, NULL, N_THREADS);
    
    for (int i = 0; i < N_THREADS; i++) {
        args[i].starting_row = WORK_LOAD * i;
        args[i].barrier = &barrier;
        args[i].converged = converged;
        args[i].all_converged = &all_converged;
        args[i].thread_id = i;
        args[i].iterations = iterations;
        pthread_create(&threads[i], NULL, thread_simulate, &args[i]);
    }

    for (int i = 0; i < N_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Iterations: %d\n", args[0].iterations);
    pthread_barrier_destroy(&barrier);
    return 0;
}
