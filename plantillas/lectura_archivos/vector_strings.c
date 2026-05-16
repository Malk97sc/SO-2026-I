#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 256

void showtree();

void read_file(const char *file, char ***vec, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    char **vec; 
    int size;

    read_file(argv[1], &vec, &size);

    for(int i=0; i < size; i++){
        free(vec[i]);
    }
    free(vec);
    vec = NULL;
    return EXIT_SUCCESS;
}

void read_file(const char *file, char ***vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    char buffer[MAX_LEN];

    fscanf(fl, "%d", size);
    fgetc(fl);
    printf("Size: %d\n", *size);

    *vec = (char**) malloc(*size * sizeof(char *));
    if(!(*vec)){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        fgets(buffer, sizeof(buffer), fl);
        buffer[strcspn(buffer, "\n")] = '\0';
        (*vec)[i] = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy((*vec)[i], buffer);
    }

    for(int i=0; i < *size; i++){
        printf("%s: and len %ld\n", (*vec)[i], strlen((*vec)[i]));
    }

    fclose(fl);
}