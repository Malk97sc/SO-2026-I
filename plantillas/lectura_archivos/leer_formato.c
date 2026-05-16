#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 256 

void showtree();

void read_file(const char *file, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    int n_elements;
    read_file(argv[1], &n_elements);

    return EXIT_SUCCESS;
}

void read_file(const char *file, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }
    int id;
    char name[MAX_LEN];
    float value;

    //Si no se sabe la cantidad de elementos a leer, usar esta
    /*while(fscanf(fl, "%d,%s,%f", &id, name, &value) > 0){
        printf("ID: %d\n", id);
        printf("Name: %s\n", name);
        printf("Value: %f\n", value);
    }*/ 

    //Si se conoce la cantidad de elementos a leer, usar esta
    fscanf(fl, "%d", size);

    for(int i=0; i < *size; i++){
        fscanf(fl, "%d,%[^,],%f", &id, name, &value);
        printf("ID: %d\n", id);
        printf("Name: %s\n", name);
        printf("Value: %f\n", value);
    }

    fclose(fl);
}