#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void showtree();

void read_file(const char *file, int **vec, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    int size;
    int *vec;

    read_file(argv[1], &vec, &size);

    //TU CODIGO

    free(vec);
    return EXIT_SUCCESS;
}

void read_file(const char *file, int **vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", size);
    printf("Size: %d\n", *size);

    *vec = (int*) malloc(*size * sizeof(int));
    if(!*vec){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        if(fscanf(fl, "%d", &(*vec)[i]) != 1){
            fprintf(stderr, "Error: no se pudo leer el elemento %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0; i < *size; i++){
        printf("%d\n", (*vec)[i]);
    }

    fclose(fl);
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}