/*
 * Exercise 2: Save username in config.log
 */
#include <stdio.h>

int main(){
    char name[50];
    printf("Enter your name: ");
    scanf("%s", name);

    FILE *f = fopen("config.log", "a");
    if(!f){
        printf("Log opening error\n");
        return 1;
    }

    fprintf(f, "Registered user: %s\n", name);
    printf("Name saved in config.log\n");

    fclose(f);
    return 0;
}
