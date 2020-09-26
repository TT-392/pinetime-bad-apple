#include <stdint.h>
#include <stdio.h>
#include "testMatrix.c"
#include <stdlib.h>

void printMatrix(int matrix[10][10]) {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 10; x++) {
            if (matrix[y][x] & 1  == 1){
                printf("■ ");
            } else if (matrix[y][x] == 2) {
                printf("o ");
            } else {
                printf("□ ");
            }
        }
        printf("\n");
    }
}

int main() {
    int intMatrix[10][10];

    for (int i = 0; i < 100; i++){
        intMatrix[i%10][i/10] = matrix[i%10][i/10];
    }

    int slowdown = 8;
    int x1 = 0; int x2 = 0; int y1 = 0; int y2 = 0;

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
           if (intMatrix[i][j] == 1)
               if (!y1)
                   y1 = i + 1;
           if (intMatrix[9-i][j] == 1)
               if (!y2)
                   y2 = i + 1;
           if (intMatrix[j][i] == 1)
               if (!x1)
                   x1 = i + 1;
           if (intMatrix[j][9-i] == 1)
               if (!x2)
                   x2 = i + 1;
        }
        for (int j = 0; j < 10; j++) {
            if (!y1)
                intMatrix[i][j] = 2;
            if (!y2)
                intMatrix[9-i][j] = 2;
            if (!x1)
                intMatrix[j][i] = 2;
            if (!x2)
                intMatrix[j][9-i] = 2;
        }
    }
    printMatrix(intMatrix);

    printf("x1 = %i, x2 = %i, y1 = %i, y2 = %i", x1-1, x2-1, y1-1, y2-1);
}
