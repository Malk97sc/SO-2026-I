#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int id;
    int start;
    int end;
} thread_args;

int **matrix;
double **results;
int rows, cols;

pthread_mutex_t lock;

void create_threads(int n_threads);
void read_file(const char *file, int ***mtx, int *rows, int *cols);

void *func(void *arg);
int la_formula(int i, int j);

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    int n_threads = atoi(argv[2]);

    read_file(argv[1], &matrix, &rows, &cols);

    results = (double**) calloc(rows, sizeof(double*));
    if(!results){
        perror("Fail Calloc");
        exit(-1);
    }

    for(int i=0; i < rows; i++){
        results[i] = (double*) calloc(cols, sizeof(double));
        if(!results[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    create_threads(n_threads);

    printf("\nResults:\n");
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%lf ", results[i][j]);
        }
        printf("\n");
    }


    for(int i=0; i < rows; i++){
        free(matrix[i]);
        free(results[i]);
    }
    free(results);
    free(matrix);
    return EXIT_SUCCESS;
}

void create_threads(int n_threads){
    pthread_t *threads; //n_threads
    thread_args *args;
    int chunk;

    threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    args = (thread_args*) malloc(n_threads * sizeof(thread_args));
    if(!threads || !args){
        perror("Fail malloc");
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);

    chunk = rows / n_threads;

    for(int i=0; i < n_threads; i++){
        args[i].id = i;
        args[i].start = chunk * i;
        args[i].end = (i == n_threads-1) ? rows : chunk * (i+1);

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
    int idx_start, idx_end, sum = 0;
    printf("Thread: %d\n", thread->id);

    idx_start = thread->start;
    idx_end = thread->end;

    for(int i=idx_start; i < idx_end; i++){
        for(int j=0; j < cols; j++){
            sum = la_formula(i, j);
            //printf("Formula return (i %d, j %d): %d\n",i,j, sum);
            pthread_mutex_lock(&lock);
            results[i][j] = sum / 9.0;
            pthread_mutex_unlock(&lock);
        }
        sum = 0;        
    }

    return NULL;
}

int la_formula(int i, int j){
    int sum = 0;
    for(int s=i-1; s <= i+1; s++){
        for(int t=j-1; t <= j+1; t++){
            if(s >= 0 && s < rows && t >= 0 && t < cols){
                sum += matrix[s][t];
            }
        }
    }
    return sum;
}

void read_file(const char *file, int ***mtx, int *rows, int *cols){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fscanf(fl, "%d", rows);
    fscanf(fl, "%d", cols);
    printf("Rows: %d - Cols: %d\n", *rows, *cols);

    *mtx = (int **) malloc(*rows * sizeof(int*));
    if(!(*mtx)){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < *rows; i++){
        (*mtx)[i] = (int*) malloc(*cols * sizeof(int));
        if(!(*mtx)[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            fscanf(fl, "%d", &(*mtx)[i][j]);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            printf("%d ", (*mtx)[i][j]);
        }
        printf("\n");
    }

    fclose(fl);
}