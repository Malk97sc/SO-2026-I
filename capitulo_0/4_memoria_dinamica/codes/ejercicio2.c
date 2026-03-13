/*
 * Exercise 2: Expansion with realloc
 */
#include <stdio.h>
#include <stdlib.h>

int main(){
    int start_size = 2, end_size = 4;
    int *arr, *temp;
    
    arr = (int*)malloc(start_size * sizeof(int));
    if(!arr) return 1;

    arr[0] = 10;
    arr[1] = 20;

    printf("Size %d: %d, %d\n", start_size, arr[0], arr[1]);

    // Expand to end_size
    temp = (int*)realloc(arr, end_size * sizeof(int));
    if(!temp){
        free(arr);
        return 1;
    }
    arr = temp;

    arr[2] = 30;
    arr[3] = 40;

    printf("Size %d: %d, %d, %d, %d\n", end_size, arr[0], arr[1], arr[2], arr[3]);

    free(arr);
    return EXIT_SUCCESS;
}
