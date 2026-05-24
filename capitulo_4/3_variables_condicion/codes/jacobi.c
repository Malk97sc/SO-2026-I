#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int id;
    int start, end;
} thread_args;

int **mtx, **next, **results;
int rows, cols;

int n_threads;
int interval_t;

int ready, exit_cd, show_mtx;

pthread_mutex_t lock;
pthread_cond_t cond;

void create_threads(int n_threads);
void read_file(const char *file, int ***mtx, int *rows, int *cols);
void show_results();

void show_matrix(int **mtx, int rows, int cols);
void swap();
void copy_mtx(int **dest, int **src);

int search_neigh(int dx, int dy);

void *func(void *arg);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }

    n_threads = atoi(argv[2]);
    interval_t = atoi(argv[3]);

    ready = exit_cd = show_mtx = 0;

    read_file(argv[1], &mtx, &rows, &cols);
    next = (int**) calloc(rows, sizeof(int*));
    results = (int**) calloc(rows, sizeof(int*));

    for(int i=0; i < rows; i++){
        next[i] = (int*) calloc(cols, sizeof(int));
        results[i] = (int*) calloc(cols, sizeof(int));

        for(int j=0; j < cols; j++){
            next[i][j] = mtx[i][j];
            results[i][j] = mtx[i][j];
        }
    }

    create_threads(n_threads);

    for(int i=0; i < rows; i++){
        free(next[i]);
        free(mtx[i]);
        free(results[i]);
    }
    free(next);
    free(mtx);
    free(results);
    return EXIT_SUCCESS;
}

void create_threads(int n_threads){
    int chunk;
    pthread_t *threads; //n_threads
    thread_args *args;

    threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    args = (thread_args*) malloc(n_threads * sizeof(thread_args));
    if(!threads || !args){
        perror("Fail malloc");
        exit(-1);
    }

    pthread_mutex_init(&lock, NULL);
    pthread_cond_init(&cond, NULL);

    chunk = (rows - 2) / n_threads;

    for(int i=0; i < n_threads; i++){
        args[i].id = i;
        args[i].start = 1 + (chunk * i);
        args[i].end = (i == n_threads-1) ? rows - 2 : args[i].start + chunk - 1;
        pthread_create(&threads[i], NULL, func, &args[i]);
    }

    show_results();

    for(int i=0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    pthread_cond_destroy(&cond);
    free(threads);
    free(args);
}

void *func(void *arg){
    thread_args *t = (thread_args*) arg;
    int idx_start, idx_end;

    idx_start = t->start;
    idx_end = t->end;

    while(1){
        for(int i= idx_start; i <= idx_end; i++){
            for(int j=1; j < cols-1; j++){
                next[i][j] = search_neigh(i, j);
            }
        }        
        pthread_mutex_lock(&lock);
        ready++;

        if(ready == n_threads){
            swap();
            copy_mtx(results, mtx);

            ready = 0;
            show_mtx = 1;
            pthread_cond_broadcast(&cond);
        }

        while(show_mtx && !exit_cd){
            pthread_cond_wait(&cond, &lock);
        }

        if(exit_cd){
            pthread_mutex_unlock(&lock);
            break;
        }

        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

void show_results(){
    for(int t=0; t < interval_t; t++){
        pthread_mutex_lock(&lock);
        while(!show_mtx){
            pthread_cond_wait(&cond, &lock);
        }

        printf("\nITERATION: %d\n", t);

        show_matrix(results, rows, cols);

        show_mtx = 0;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lock);
    }

    pthread_mutex_lock(&lock);
    exit_cd = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&lock);

}

int search_neigh(int dx, int dy){
    int left = mtx[dx -1][dy];
    int right = mtx[dx+1][dy];

    int up = mtx[dx][dy-1];
    int down = mtx[dx][dy+1];

    return (left + right + up + down) / 4;
}

void show_matrix(int **mtx, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%d\t", mtx[i][j]);
        }    
        printf("\n");
    }
}

void swap(){
    int **temp = mtx;
    mtx = next;
    next = temp;
}

void copy_mtx(int **dest, int **src){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            dest[i][j] = src[i][j];
        }    
    }
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