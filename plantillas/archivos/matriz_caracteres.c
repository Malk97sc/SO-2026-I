#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 256

void showtree();

void read_file(const char *file, char ***mtx, int *rows, int *cols);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    char **mtx;
    int rows, cols;

    read_file(argv[1], &mtx, &rows, &cols);

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%c ", mtx[i][j]);
        }
        printf("\n");
    }

    for(int i=0; i < rows; i++){
        free(mtx[i]);
    }
    free(mtx);
    mtx = NULL;
    return EXIT_SUCCESS;
}

void read_file(const char *file, char ***mtx, int *rows, int *cols){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", rows);
    fscanf(fl, "%d", cols);
    printf("Rows: %d, Cols: %d\n", *rows, *cols);

    *mtx = (char **) malloc(*rows * sizeof(char *));
    if(!(*mtx)){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < *rows; i++){   
        (*mtx)[i] = (char*) malloc(*cols * sizeof(char));
        if(!(*mtx)[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            fscanf(fl, " %c", &(*mtx)[i][j]);
        }
    }

    fclose(fl);
}