#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

void showtree();

void read_file(const char *file, int **vec, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = 8, child_id;

    int size, chunk;
    int *vec;

    read_file(argv[1], &vec, &size);

    chunk = size / n_children;
    printf("CHUNK: %d\n", chunk);

    for(child_id = 0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //proceso PADRE
        showtree();

        for(int i=0; i < n_children; i++) wait(NULL);

        //item 6...
        
    }else{ //procesos HIJOS
        int idx_start, idx_end;

        printf("[%d]-[CHILD_ID] = %d\n", getpid(), child_id);
        idx_start = chunk * child_id;
        /*if(child_id == n_children-1){
            idx_end = size;
        }else{
            idx_end = chunk * (child_id+1);
        }*/
        idx_end = child_id == n_children-1 ? size : chunk * (child_id+1);
        printf("[%d]-[IDX] = [%d][%d]\n\n", getpid(), idx_start, idx_end);

        for(int i=idx_start; i < idx_end; i++){
            //item 2 suma hijo
        }

        //item 3 out.txt

    }

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
        //printf("%d ", (*vec)[i]);
    }

    fclose(fl);
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}