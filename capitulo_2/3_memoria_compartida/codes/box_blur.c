#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void showtree();

unsigned int sizeof_dm(int rows, int cols, size_t size_elements);
void create_index(void **matrix, int rows, int cols, size_t size_elements);
void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements);


void read_file(const char *file, double ***mtx, int *rows, int *cols);
void copy_matrix(double **A, double **B, int rows, int cols);
double blur_filter(double **mtx, int rows, int cols, int dx, int dy);

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send file and n_children");
        return EXIT_SUCCESS;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[2]), child_id;

    double **data_mtx, **results_mtx, **aux;
    int rows, cols;
    int data_shm_id, results_shm_id;
    int chunk;

    read_file(argv[1], &aux, &rows, &cols);

    create_shm(&data_shm_id, (void **)&data_mtx, rows, cols, sizeof(double)); //datos
    create_shm(&results_shm_id, (void **)&results_mtx, rows, cols, sizeof(double)); //resultados
    copy_matrix(data_mtx, aux, rows, cols);
    
    for(int i=0; i < rows; i++) free(aux[i]);
    free(aux);

    chunk = rows / n_children;

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //padre   
        for(int i=0; i < n_children; i++) wait(NULL);
        printf("ORIGINAL\n");
        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                printf("%lf ", data_mtx[i][j]);
            }
            printf("\n");
        }
        printf("\n");
        printf("BLUR\n");
        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                printf("%lf ", results_mtx[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        shmdt(data_mtx);
        shmctl(data_shm_id, IPC_RMID, NULL);
        shmdt(results_mtx);
        shmctl(results_shm_id, IPC_RMID, NULL);
    }else{ //hijos
        sleep(child_id);
        int idx_start, idx_end;
        double result = 0;

        idx_start = chunk * child_id;
        idx_end = (child_id == n_children-1) ? rows : chunk * (child_id+1);

        for(int i=idx_start; i < idx_end; i++){
            for(int j=0; j < cols; j++){
                result = blur_filter(data_mtx, rows, cols, i, j);
                //printf("Result: %lf\n", result);
                results_mtx[i][j] = result;
            }
        }

        shmdt(data_mtx);
        shmdt(results_mtx);
    }

    
    return EXIT_SUCCESS;
}

double blur_filter(double **mtx, int rows, int cols, int dx, int dy){
    double sum = 0;
    int count = 0;

    for(int i=dx-1; i <= dx+1; i++){
        for(int j=dy-1; j <= dy+1; j++){
            if(i >= 0 && i < rows && j >= 0 && j < cols){
                //printf("Value: %lf\n", mtx[i][j]);
                sum += mtx[i][j];
                count++;
            }
        }
    }

    //printf("Count: %d\n", count);
    return sum / count;
}

void read_file(const char *file, double ***mtx, int *rows, int *cols){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fscanf(fl, "%d", rows);
    fscanf(fl, "%d", cols);
    printf("Rows: %d - Cols: %d\n", *rows, *cols);

    *mtx = (double **) malloc(*rows * sizeof(double*));
    if(!(*mtx)){
        perror("Fail malloc\n");
        exit(1);
    }

    for(int i=0; i < *rows; i++){
        (*mtx)[i] = (double*) malloc(*cols * sizeof(double));
        if(!(*mtx)[i]){
            perror("Fail malloc cols\n");
            exit(1);
        }
    }

    for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            fscanf(fl, "%lf", &(*mtx)[i][j]);
        }
    }

    /*for(int i=0; i < *rows; i++){
        for(int j=0; j < *cols; j++){
            printf("%lf ", (*mtx)[i][j]);
        }
        printf("\n");
    }*/

    fclose(fl);
}

void copy_matrix(double **A, double **B, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            A[i][j] = B[i][j];
        }
    }
}

void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements){
    size_t size_mtx;

    size_mtx = sizeof_dm(rows, cols, sizeof(size_elements));
    *shm_id = shmget(IPC_PRIVATE, size_mtx, 0666 | IPC_CREAT);
    if(*shm_id == -1){
        perror("Fail shmget");
        exit(1);
    }

    *mtx = shmat(*shm_id, NULL, 0);
    if(*mtx == NULL){
        perror("Fail Shmat");
        exit(1);
    }

    create_index((void*)*mtx, rows, cols, size_elements);
}

unsigned int sizeof_dm(int rows, int cols, size_t size_elements){
    size_t size = rows * sizeof(void *);
    size += (rows * cols * size_elements); 
    return size;
}

void create_index(void **matrix, int rows, int cols, size_t size_elements){
    int i;
    size_t size_rows = cols * size_elements;
    matrix[0] = matrix + rows;
    for(i = 1; i < rows; i++){
        matrix[i] = (matrix[i-1] + size_rows);
    }
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}