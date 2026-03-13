/*
 * Exercise 1: Count lines in data.txt
 */
#include <stdio.h>

int main(int argc, char **argv){
    if(argc < 2){
        printf("Usage: %s <filename>\n", argv[0]);
        return EXIT_FAILURE;
    }
    int lines = 0;
    char ch;
    FILE *f = fopen(argv[1], "r");

    if(!f){
        printf("Could not open %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    while((ch = fgetc(f)) != EOF){ //EOF = End Of File
        if(ch == '\n'){
            lines++;
        }
    }

    printf("The file has %d lines\n", lines);

    fclose(f);
    return 0;
}
