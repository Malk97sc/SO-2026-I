/*
 * Exercise 1: Area of a circle
 */
#include <stdio.h>

#define PI 3.14159

float calculateArea(float radius){
    return PI * radius * radius;
}

int main(){
    float r;
    
    printf("Enter radius: ");
    scanf("%f", &r);
    
    printf("The area is: %.2f\n", calculateArea(r));

    return EXIT_SUCCESS;
}
