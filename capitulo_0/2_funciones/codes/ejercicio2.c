/*
 * Exercise 2: Even or Odd without return
 */
#include <stdio.h>

void checkParity(int n){
    if(n % 2 == 0){
        printf("%d is even\n", n);
    }else{
        printf("%d is odd\n", n);
    }
}

int main(){
    int num;
    printf("Enter a number: ");
    scanf("%d", &num);

    checkParity(num);
    return EXIT_SUCCESS;
}
