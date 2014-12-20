#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ROW 1024
#define COL 1024

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

void simulate(float plate[ROW][COL]) {
    float* temp = (float*) malloc(sizeof(float)*ROW*COL);
    bool steady_state;
    int iterations = 0;

    do {
        steady_state = true;
        memcpy(temp, plate, ROW*COL*sizeof(float));
        int i, j;
        for (i = 1; i < ROW-1; i++) {
            for (j = 1; j < COL-1; j++) {
                if (i == 400 && j <= 330) {
                    j = 330;
                    continue;
                } else if (i == 200 && j == 500) {
                    continue;
                } else {
                    float north = temp[(i-1)*COL+j];
                    float west = temp[i*COL+(j-1)];
                    float east = temp[i*COL+(j+1)];
                    float south = temp[(i+1)*COL+j];
                    float present = 4 * temp[i*COL+j];
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
                        steady_state = false;
                        goto breakout;
                    }
                }
            }
        }
        breakout:
        iterations++;
    } while ((!steady_state) /*&& iterations >= 100*/);
    //printPlate(plate);
    printf("%d\n", iterations);
}

int main(int argc, char** argv) {
    float plate[ROW][COL];
    initializePlate(plate);
//    printPlate(plate);    
    simulate(plate);
//    printPlate(plate);    
    return 0;
}
