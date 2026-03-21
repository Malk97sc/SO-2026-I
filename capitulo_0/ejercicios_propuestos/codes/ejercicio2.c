#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void read_file(const char *file, int max);

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send file and max_MB\n");
        return EXIT_FAILURE;
    }
    float max_mb = atof(argv[2]);

    printf("MAX MEMORY MB: %f\n", max_mb);

    read_file(argv[1], max_mb);

    return EXIT_SUCCESS;
}


void read_file(const char *file, int max){
    FILE *fl = fopen(file, "r");
    if(!fl){perror("Fail fopen\n"); exit(1);}

    int id;
    float timestamp, memory_consumption;
    int n;

    fscanf(fl, "%d", &n);

    for(int i=0; i < n; i++){
        fscanf(fl, " %d %f %f", &id, &timestamp, &memory_consumption);
        if(memory_consumption > max){
            printf("%d %.3f %.3f\n", id, timestamp, memory_consumption);
        }
    }
    
    fclose(fl);
}