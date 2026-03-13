/*
 * Exercise 1: Even numbers from 1 to 100
 */
#include <stdio.h>

int main(){
    int max = 100;
    for(int i = 1; i <= max; i++){
        if(i % 2 == 0){
            printf("%d ", i);
        }
    }
    printf("\n");
    return EXIT_SUCCESS;
}
