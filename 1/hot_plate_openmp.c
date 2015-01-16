#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW 1024
#define COL 1024

//float plate[ROW][COL];
//float copy[ROW][COL];

void printPlate(float plate[ROW][COL]) {
    int i, j;
    for (i = 0; i < ROW; i++) {
        for (j = 0; j < COL; j++) {
            printf("%.1f ", plate[i][j]);
        }
        printf("\n");
    }
}

void initializePlate(float plate[ROW][COL]) {
    int i, j;
    
    //initialize top row
    for (j = 0; j < COL; j++) {
        plate[0][j] = 0;
    }

    //initialize bottom row
    for (j = 0; j < COL; j++) {
        plate[ROW-1][j] = 100;
    }
    
    //initialize main body
    for (i = 1; i < ROW-1; i++) {
        for (j = 0; j < COL; j++) {
            if (j == 0 || j == COL-1) {
                plate[i][j] = 0;
            } else {
                plate[i][j] = 50;
            }
        }
    }
    
    //initialize row 400
    for (j = 0; j < 331; j++) {
        plate[400][j] = 100;
    }

    //initialize (200,500)
    plate[200][500] = 100;
    
}

void simulate(float plate[ROW][COL], float copy[ROW][COL]) {
    //float* temp = (float*) malloc(sizeof(float)*ROW*COL);
    bool flucuating = false;
    int iterations = 0;

    do {
        flucuating = false;
        memcpy(copy, plate, ROW*COL*sizeof(float));
        int i, j;
        for (i = 1; i < ROW-1; i++) {
            for (j = 1; j < COL-1; j++) {
                if (i == 400 && j <= 330) {
                    j = 330;
                    continue;
                } else if (i == 200 && j == 500) {
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

        for (i = 2; i < ROW-2; i++) {
            for (j = 2; j < COL-2; j++) {
                if (i == 400 && j <= 330) {
                    j = 330;
                    continue;
                } else if (i == 200 && j == 500) {
                    continue;
                } else {
                    float north = plate[i-1][j];
                    float west = plate[i][j-1];
                    float east = plate[i][j+1];
                    float south = plate[i+1][j];
                    float average = (north + west + east + south) / 4;
                    if (fabsf(plate[i][j] - average) >= 0.1) {
                        flucuating = true;
                        goto breakout;
                    }
                }
            }
        }
        breakout:
        iterations++;
    } while (flucuating /*&& iterations >= 100*/);
    //printPlate(plate);
    printf("%d\n", iterations);
}

int main(int argc, char** argv) {
    static float plate[ROW][COL];
    static float copy[ROW][COL];
//    float *plate = (float*) malloc(sizeof(float)*ROW*COL);
    initializePlate(plate);
//    printPlate(plate);    
    simulate(plate, copy);
//    printPlate(plate);    
    return 0;
}
