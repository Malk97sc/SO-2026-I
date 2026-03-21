#include <stdio.h>
#include <stdlib.h>

int main(){
    FILE *file = fopen("numbers.txt", "r");
    if(!file) return 1;

    int *data = NULL;
    int count = 0;
    int num;

    while(fscanf(file, "%d", &num) == 1){
        count++;
        data = (int *)realloc(data, count * sizeof(int));
        data[count - 1] = num;
    }
    fclose(file);

    long sum = 0;
    for(int i = 0; i < count; i++){
        sum += data[i];
    }

    printf("Count: %d\n", count);
    printf("Sum: %ld\n", sum);
    printf("Average: %.2f\n", (float)sum / count);

    free(data);
    return 0;
}
