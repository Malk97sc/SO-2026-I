#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef enum {
    DEFORESTED,
    FOREST,
    REGENERATE
} Terrain;

typedef struct {
    int id;
    //fill
} thread_args;

pthread_barrier_t barrier;
pthread_mutex_t mutex;

int **matrix = NULL, **tmp = NULL;
int rows, cols, years;

void create_threads(int n_threads);
void read_file(const char *file, int ***mtx, int *rows, int *cols, int *years);

void *forest2deforested(void *arg); //th1
void *deforested2regenerate(void *arg); //th2

int count_neigh(int dx, int dy, int state);
void update_data();

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file");
        return EXIT_FAILURE;
    }

    int n_threads = 2;

    rows = cols = years = 0;

    read_file(argv[1], &matrix, &rows, &cols, &years);

    tmp = (int **) malloc(rows * sizeof(int*));
    if(!tmp){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < rows; i++){
        tmp[i] = (int*) malloc(cols * sizeof(int));
        if(!tmp[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            tmp[i][j] = matrix[i][j];
        }
    }

    create_threads(n_threads);

    for(int i=0; i < rows; i++){
        free(tmp[i]);
        free(matrix[i]);
    }
    free(matrix);
    free(tmp);
    return EXIT_SUCCESS;
}

void create_threads(int n_threads){
    pthread_t th1, th2;

    pthread_barrier_init(&barrier, NULL, n_threads+1);
    pthread_mutex_init(&mutex, NULL);

    pthread_create(&th1, NULL, forest2deforested, NULL);
    pthread_create(&th2, NULL, deforested2regenerate, NULL);

    for(int y=0; y < years; y++){
        pthread_barrier_wait(&barrier);

        printf("\nYear: %d\n", y+1);
        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        pthread_mutex_lock(&mutex);
        update_data();
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier);
    }

    pthread_join(th1, NULL);
    pthread_join(th2, NULL);

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);
}

int count_neigh(int dx, int dy, int state){
    int count = 0;
    for(int i=dx-1; i <= dx+1; i++){
        for(int j=dy-1; j <= dy+1; j++){
            if(i == dx && j == dy) continue;

            if(i >= 0 && i < rows && j >= 0 && j < cols){
                if(matrix[i][j] == state){
                    count++;
                }
            }
        }
    }
    return count;
}

void *forest2deforested(void *arg){
    int neigh_def = 0;

    for(int y=0; y < years; y++){
        pthread_barrier_wait(&barrier);
        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                if(matrix[i][j] == FOREST){
                    neigh_def = count_neigh(i, j, DEFORESTED);
                    if(neigh_def >= 4){
                        pthread_mutex_lock(&mutex);
                        //printf("Change forest2deforested in: [%d][%d]\n", i, j);
                        tmp[i][j] = DEFORESTED;
                        pthread_mutex_unlock(&mutex);
                    }
                }
                neigh_def = 0;
            }            
        }
        pthread_barrier_wait(&barrier);
    }
}

void *deforested2regenerate(void *arg){
    int neigh_reg = 0;

    for(int y=0; y < years; y++){
        pthread_barrier_wait(&barrier);
        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                if(matrix[i][j] == DEFORESTED){
                    neigh_reg = count_neigh(i, j, REGENERATE);
                    if(neigh_reg >= 5){
                        pthread_mutex_lock(&mutex);
                        tmp[i][j] = REGENERATE;
                        pthread_mutex_unlock(&mutex);
                    }
                }
                neigh_reg = 0;
            }
        }
        pthread_barrier_wait(&barrier);
    }
}

void update_data(){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            matrix[i][j] = tmp[i][j];
        }
    }            
}

void read_file(const char *file, int ***mtx, int *rows, int *cols, int *years){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fscanf(fl, "%d", years);
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

    /*for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            printf("%d ", (*mtx)[i][j]);
        }
        printf("\n");
    }*/

    fclose(fl);
}