#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createMatrix(char ***mtx, int rows, int columns);
void Free(char ***mtx, int rows);

int main(int argc, char **argv){
    if(argc < 4){
        perror("Send the size NxM and symbol\n");
        return EXIT_FAILURE;
    }
    char **mtx;
    int rows, columns;
    char symbol, point = '.';

    symbol = argv[3][0]; //first position of the char *argv[3]
    rows = atoi(argv[1]), columns = atoi(argv[2]);
    createMatrix(&mtx, rows, columns);

    printf("Matrix %d x %d created successfully.\n", rows, columns);

    for(int i=0; i < rows; i++){
        for(int j=0; j < columns; j++){
            if(i == 0 || i == rows-1 || j == 0 || j == columns-1){
                mtx[i][j] = symbol;
            }else{
                mtx[i][j] = point;
            }
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < columns; j++){
            printf("%c ", mtx[i][j]);
        }
        printf("\n");
    }

    Free(&mtx, rows);
    return EXIT_SUCCESS;
}

void createMatrix(char ***mtx, int rows, int columns){
    *mtx = (char**) malloc(rows * sizeof(char*));
    if(!(*mtx)){ 
        perror("Failed to allocate memory for matrix rows");
        exit(1);
    }

    for(int i=0; i < rows; i++){
        (*mtx)[i] = (char*) malloc(columns * sizeof(char));
        if(!(*mtx)[i]){
            perror("Failed to allocate memory for matrix columns");
            exit(1);
        }
    }
}

void Free(char ***mtx, int rows){
    for(int i=0; i < rows; i++){
        free((*mtx)[i]); 
        (*mtx)[i] = NULL;
    }
    free((*mtx));
    *mtx = NULL;
}