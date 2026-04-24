#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <sys/types.h>

#define MAX_LEN 256

typedef struct ADN_statistics{
    int A, T, C, G;
}ADN_statistics;

typedef struct Found{
    int child_id, pos;
}Found;

void showtree();

void read_file(const char *file, char **vec, int *size);

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send a file and n_children\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[2]), child_id;
    char *vec;
    int size;

    int pipes[n_children][2];
    int chunk;

    read_file(argv[1], &vec, &size);

    for(int i=0; i < n_children; i++){
        pipe(pipes[i]);
    }

    chunk = size / (n_children-1);

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //padre
        for(int i=0; i < n_children; i++) close(pipes[i][1]);
        for(int i=0; i < n_children; i++) wait(NULL);


        printf("\nPARENT PROCESS:\n");
        for(int i=0; i < n_children; i++){
            if(i == 0){
                ADN_statistics data;
                read(pipes[i][0], &data, sizeof(ADN_statistics));
                printf("Statistics: \nA: %d\nT: %d\nC: %d\nG: %d\n\n", data.A, data.T, data.C, data.G);
            }else{
                Found data;
                read(pipes[i][0], &data, sizeof(Found));
                printf("Child %d found pattern in %d\n", data.child_id, data.pos);
            }
        }

        for(int i=0; i < n_children; i++) close(pipes[i][0]);

    }else{ //hijos
        for(int i=0; i < n_children; i++) close(pipes[i][0]);

        if(child_id == 0){
            ADN_statistics stats = {0};

            for(int i=0; i < size; i++){
                switch(vec[i]){
                    case 'A':{
                        stats.A++; 
                        break;
                    }
                    case 'T':{
                        stats.T++;
                        break;
                    }
                    case 'C':{
                        stats.C++;
                        break;
                    }
                    case 'G':{
                        stats.G++;
                        break;
                    }
                }
            }
            write(pipes[child_id][1], &stats, sizeof(ADN_statistics));
        }else{
            int idx_start, idx_end;
            const char *pattern = "TTGTAC";
            int pattern_len = strlen(pattern);
            Found found_p = {0};

            idx_start = chunk * (child_id-1);
            idx_end = (child_id == n_children-1) ? size : chunk + idx_start;

            //printf("Child %d searching from %d to %d\n", child_id, idx_start, idx_end);

            for(int i=idx_start; i < idx_end; i++){
                if(vec[i] == 'T'){
                    if(strncmp(&vec[i], pattern, pattern_len) == 0){
                        found_p.child_id = child_id;
                        found_p.pos = i;
                        write(pipes[child_id][1], &found_p, sizeof(Found));
                    }
                }
            }
        }

        for(int i=0; i < n_children; i++) close(pipes[i][1]);
    }


    free(vec);
    vec = NULL;
    return EXIT_SUCCESS;
}

void read_file(const char *file, char **vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", size);
    printf("n bases: %d\n", *size);
    fgetc(fl);

    *vec = (char*) malloc(*size * sizeof(char));
    if(!*vec){
        perror("Fail Malloc");
        exit(1);
    }

    for(int i=0; i < *size; i++){
        fscanf(fl, "%c", &(*vec)[i]);
    }

    fclose(fl);
}