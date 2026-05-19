#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int id;
    int start;
    int end;
} thread_args;

double *vec;
int size;

double global_sum = 0;
double global_max = -1;

pthread_mutex_t lock;
pthread_barrier_t barrier;

void create_threads(int n_threads);
void read_file(const char *file, double **vec, int *size);

void *func(void *arg);

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    int n_threads = atoi(argv[2]);

    read_file(argv[1], &vec, &size);

    create_threads(n_threads);

    printf("\nResults\n");
    for(int i=0; i < size; i++){
        printf("%lf\n", vec[i]);
    }

    free(vec);
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
    pthread_barrier_init(&barrier, NULL, n_threads);

    chunk = size / n_threads;

    for(int i=0; i < n_threads; i++){
        args[i].id = i;
        args[i].start = chunk * i;
        args[i].end = (i == n_threads-1) ? size : chunk * (i+1);
        pthread_create(&threads[i], NULL, func, &args[i]);
    }

    for(int i=0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_barrier_destroy(&barrier);
    free(threads);
    free(args);
}

void *func(void *arg){
    thread_args *thread = (thread_args*) arg;
    double local_sum = 0, local_max = 0;
    int idx_start, idx_end;
    //printf("Thread: %d\n", thread->id);

    idx_start = thread->start;
    idx_end = thread->end;

    for(int i=idx_start; i < idx_end; i++){
        local_sum += vec[i];
    }

    pthread_mutex_lock(&lock);
    global_sum += local_sum;
    pthread_mutex_unlock(&lock);

    pthread_barrier_wait(&barrier); //todos los hilos esperan

    for(int i=idx_start; i < idx_end; i++){
        vec[i] /= global_sum;
    }

    pthread_barrier_wait(&barrier);
    if(thread->id == 0){
        printf("\nFASE 1\n");
        for(int i=0; i < size; i++){
            printf("%lf\n", vec[i]);
        }
    }
    
    for(int i=idx_start; i < idx_end; i++){
        if(vec[i] > local_max) local_max = vec[i];
    }

    pthread_mutex_lock(&lock);
    if(local_max > global_max) global_max = local_max;
    pthread_mutex_unlock(&lock);

    pthread_barrier_wait(&barrier);
    if(thread->id == 0){
        printf("\nFASE 2\n");
        printf("MAX: %lf\n", global_max);
    }

    for(int i=idx_start; i < idx_end; i++){
        vec[i] /= global_max;
    }

    pthread_barrier_wait(&barrier);
    if(thread->id == 0){
        printf("\nFASE 3\n");
        for(int i=0; i < size; i++){
            printf("%lf\n", vec[i]);
        }
    }

    return NULL;
}

void read_file(const char *file, double **vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", size);
    printf("Size: %d\n", *size);

    *vec = (double*) malloc(*size * sizeof(double));
    if(!*vec){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        if(fscanf(fl, "%lf", &(*vec)[i]) != 1){
            fprintf(stderr, "Error: no se pudo leer el elemento %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0; i < *size; i++){
        printf("%lf\n", (*vec)[i]);
    }

    fclose(fl);
}