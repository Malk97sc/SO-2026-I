/*
 * Exercise 2: Print memory addresses
 */
#include <stdio.h>

int main(){
    int var = 42;
    int *ptr = &var;

    printf("Address of var: %p\n", (void*)&var);
    printf("Address stored in ptr: %p\n", (void*)ptr);
    printf("Address of ptr pointer: %p\n", (void*)&ptr);

    return EXIT_SUCCESS;
}
