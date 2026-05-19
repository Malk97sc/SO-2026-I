#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int id;
    //fill
} thread_args;

pthread_mutex_t lock;

void create_threads(int n_threads);

void *func(void *arg);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    int n_threads = atoi(argv[1]);

    create_threads(n_threads);

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


    pthread_mutex_lock(&lock);
    printf("Thread: %d\n", thread->id);
    pthread_mutex_unlock(&lock);

    return NULL;
}