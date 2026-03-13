#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv){    
    if(argc < 3){
        printf("Usage: %s <rows> <cols>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int rows, cols, **matrix;

    rows = atoi(argv[1]);
    cols = atoi(argv[2]);

    printf("rows: %d, cols: %d\n", rows, cols);

    matrix = (int **) malloc(rows * sizeof(int *));
    if(!matrix){
        printf("Fail malloc\n"); 
        return EXIT_FAILURE;
    }

    for(int i=0; i < rows; i++){
        matrix[i] = (int *) malloc(cols * sizeof(int));

        if(!matrix[i]){
            printf("Fail malloc\n"); 
            return 1;
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            matrix[i][j] = i+1;
        }         
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%d ", matrix[i][j]);
        }
        printf("\n");  
    }

    for(int i=0; i < rows; i++){
        free(matrix[i]);
    }
    free(matrix);
    matrix = NULL;

    return EXIT_SUCCESS;
}
