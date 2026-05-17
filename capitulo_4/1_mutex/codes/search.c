#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int id;
    int start;
    int end;
    int target;
} thread_args;

typedef struct {
    int pos;
    int thread_id;
} Result;

Result *results = NULL;
int global_count = 0;

int *vector = NULL;
int size_vec = 0;

pthread_mutex_t mutex;

void create_threads(int n_threads, int target);
void read_file(const char *file, int **vec, int *size);

void *search(void *arg);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    int n_threads = atoi(argv[2]);
    int target = atoi(argv[3]);

    read_file(argv[1], &vector, &size_vec);

    results = (Result*) malloc(size_vec * sizeof(Result));
    if(!results){
        perror("Fail malloc");
        exit(-1);
    }

    create_threads(n_threads, target);

    printf("\nTotal: %d\n", global_count);

    for(int i=0; i < global_count; i++){
        printf("Found in pos: %d by thread_id: %d\n", results[i].pos, results[i].thread_id);
    }

    free(results);
    return EXIT_SUCCESS;
}

void create_threads(int n_threads, int target){
    pthread_t *threads; //n_threads
    thread_args *args;

    int chunk;

    threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    args = (thread_args*) malloc(n_threads * sizeof(thread_args));
    if(!threads || !args){
        perror("Fail malloc");
        exit(-1);
    }

    pthread_mutex_init(&mutex, NULL);

    chunk = size_vec / n_threads;    
    
    for(int i=0; i < n_threads; i++){
        args[i].id = i;
        args[i].start = chunk * i;
        //args[i].end = (i == n_threads-1) ? size_vec : chunk * (i+1);
        if(i == n_threads-1){
            args[i].end = size_vec;
        }else{
            args[i].end = chunk * (i+1);
        }
        args[i].target = target;

        pthread_create(&threads[i], NULL, search, &args[i]);
    }

    for(int i=0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    free(threads);
    free(args);
}

void *search(void *arg){
    thread_args *thread = (thread_args*) arg;

    printf("Thread: %d\n", thread->id);

    for(int i=thread->start; i < thread->end; i++){
        if(vector[i] == thread->target){
            pthread_mutex_lock(&mutex);
            results[global_count].pos = i;
            results[global_count].thread_id = thread->id;

            global_count++;
            printf("Found pos: %d\n", i);
            pthread_mutex_unlock(&mutex);
        }
    }

    return NULL;
}

void read_file(const char *file, int **vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", size);
    printf("Size: %d\n", *size);

    *vec = (int*) malloc(*size * sizeof(int));
    if(!*vec){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        if(fscanf(fl, "%d", &(*vec)[i]) != 1){
            fprintf(stderr, "Error: no se pudo leer el elemento %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0; i < *size; i++){
        printf("%d\n", (*vec)[i]);
    }

    fclose(fl);
}