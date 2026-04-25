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

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send rows and cols");
        return EXIT_SUCCESS;
    }

    double **mtx;
    int rows = atoi(argv[1]), cols = atoi(argv[2]);
    int shm_id;

    create_shm(&shm_id, (void **)&mtx, rows, cols, sizeof(double));

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            mtx[i][j] = 50;
        }
    }

    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%f ", mtx[i][j]);
        }
        printf("\n");
    }

    shmdt(mtx);
    shmctl(shm_id, IPC_RMID, NULL);
    return EXIT_SUCCESS;
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