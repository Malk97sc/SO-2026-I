/*
 * Exercise 2: Check if a number is prime
 */
#include <stdio.h>

int main(){
    int num, i, count = 0;
    printf("Enter a number: ");
    scanf("%d", &num);

    if(num <= 1){
        printf("Not prime\n");
        return EXIT_FAILURE;
    }

    for(i = 1; i <= num; i++){
        if(num % i == 0){
            count++;
        }
    }

    if(count == 2){
        printf("Is prime\n");
    }else{
        printf("Not prime\n");
    }

    return EXIT_SUCCESS;
}
