#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
    int id;
    //fill
} thread_args;

int **matrix_A;
int *vec_B;
int *results;

int rows, cols;

int current_row = 0;

pthread_mutex_t lock;

void create_vec(int **vec, int size);
void create_mtx(int ***mtx, int rows, int cols);

void create_threads(int n_threads);

void *func(void *arg);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    int n_threads = atoi(argv[1]);
    rows = atoi(argv[2]);
    cols = atoi(argv[3]);

    create_vec(&vec_B, cols);
    create_vec(&results, rows);

    create_mtx(&matrix_A, rows, cols);

    create_threads(n_threads);

    printf("\nResults: \n");
    for(int i=0; i < rows; i++){
        printf("%d\n", results[i]);
    }


    for(int i=0; i < rows; i++){
        free(matrix_A[i]);
    }
    free(matrix_A);
    free(vec_B);
    free(results);
    return EXIT_SUCCESS;
}

void create_threads(int n_threads){
    pthread_t *threads; //n_threads
    thread_args *args;

    threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    args = (thread_args*) malloc(n_threads * sizeof(thread_args));
    if(!threads || !args){
        perror("Fail malloc");
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);

    for(int i=0; i < n_threads; i++){
        args[i].id = i;
        pthread_create(&threads[i], NULL, func, &args[i]);
    }

    for(int i=0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    free(threads);
    free(args);
}

void *func(void *arg){
    thread_args *thread = (thread_args*) arg;
    int my_row = 0;
    int sum = 0;
    //printf("Thread: %d\n", thread->id);

    while(1){
        pthread_mutex_lock(&lock);
        my_row = current_row;
        current_row++;
        pthread_mutex_unlock(&lock);

        if(my_row >= rows) break;

        for(int i=0; i < cols; i++){
            sum += vec_B[i] * matrix_A[my_row][i];
        }

        results[my_row] = sum;
        sum = 0;
        printf("Thread: %d, working on row: %d\n", thread->id, my_row);
        usleep(10000);
    }

    return NULL;
}

void create_vec(int **vec, int size){
    printf("Size: %d\n", size);

    *vec = (int*) malloc(size * sizeof(int));
    if(!*vec){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < size; i++){
        (*vec)[i] = rand() % 5;
    }

    /*for(int i=0; i < size; i++){
        printf("%d\n", (*vec)[i]);
    }*/
}

void create_mtx(int ***mtx, int rows, int cols){
    printf("Rows: %d - Cols: %d\n", rows, cols);

    *mtx = (int **) malloc(rows * sizeof(int*));
    if(!(*mtx)){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < rows; i++){
        (*mtx)[i] = (int*) malloc(cols * sizeof(int));
        if(!(*mtx)[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            (*mtx)[i][j] = rand() % 10;
        }
    }

    /*for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%d ", (*mtx)[i][j]);
        }
        printf("\n");
    }*/
}