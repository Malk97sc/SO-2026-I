#include <stdio.h>
#include <stdlib.h>

void calculate(int *arr, int n, int *sum, float *avg, int *max_val){
    *sum = 0;
    *max_val = arr[0];
    for(int i = 0; i < n; i++){
        *sum += arr[i];
        if(arr[i] > *max_val) *max_val = arr[i];
    }
    *avg = (float)*sum / n;
}

int main(){
    int n;
    if(scanf("%d", &n) != 1) return 1;

    int *vector = (int *)malloc(n * sizeof(int));
    if(!vector) return 1;

    for(int i = 0; i < n; i++){
        scanf("%d", &vector[i]);
    }

    int sum, max_val;
    float avg;

    calculate(vector, n, &sum, &avg, &max_val);

    printf("Sum: %d\n", sum);
    printf("Average: %.2f\n", avg);
    printf("Max: %d\n", max_val);

    free(vector);
    return 0;
}
