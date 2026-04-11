#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_LEN 256

void write_file(const char *file, int size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a number\n");
        return EXIT_FAILURE;
    }
    int n_elements = atoi(argv[1]);
    write_file("file.txt", n_elements);

    return EXIT_SUCCESS;
}

void write_file(const char *file, int size){
    FILE *fl = fopen(file, "w");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fprintf(fl, "%d\n", size);
    for(int i=0; i < size; i++){
        fprintf(fl, "%d\n", i);
    }

    printf("The file %s has been created\n", file);

    fclose(fl);
}
