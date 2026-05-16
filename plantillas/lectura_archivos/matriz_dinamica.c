#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void showtree();

void read_file(const char *file, int ***mtx, int *rows, int *cols);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE; 
    }
    int rows, cols;
    int **mtx;

    read_file(argv[1], &mtx, &rows, &cols);

    //TU CODIGO

    for(int i=0; i < rows; i++){
        free(mtx[i]);
    }
    free(mtx);
    return EXIT_SUCCESS;
}

void read_file(const char *file, int ***mtx, int *rows, int *cols){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fscanf(fl, "%d", rows);
    fscanf(fl, "%d", cols);
    printf("Rows: %d - Cols: %d\n", *rows, *cols);

    *mtx = (int **) malloc(*rows * sizeof(int*));
    if(!(*mtx)){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < *rows; i++){
        (*mtx)[i] = (int*) malloc(*cols * sizeof(int));
        if(!(*mtx)[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            fscanf(fl, "%d", &(*mtx)[i][j]);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            printf("%d ", (*mtx)[i][j]);
        }
        printf("\n");
    }

    fclose(fl);
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}