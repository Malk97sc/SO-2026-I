/*
 * Exercise 1: Average of N floats (Dynamic Memory)
 */
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: %s <number>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int n;
    float *arr, sum = 0;

    n = atoi(argv[1]);

    arr = (float*)malloc(n * sizeof(float));
    if(!arr){
        printf("Memory error\n");
        return 1;
    }

    for(int i = 0; i < n; i++){
        printf("Value %d: ", i+1);
        scanf("%f", &arr[i]);
        sum += arr[i];
    }

    printf("Average: %.2f\n", sum/n);

    free(arr);
    return EXIT_SUCCESS;
}
