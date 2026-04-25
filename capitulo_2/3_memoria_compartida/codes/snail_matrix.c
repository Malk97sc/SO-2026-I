#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <math.h>

void showtree();

unsigned int sizeof_dm(int rows, int cols, size_t size_elements);
void create_index(void **matrix, int rows, int cols, size_t size_elements);
void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements);

void show_matrix(int **mtx, int rows, int cols);
int mult_matx(int **A, int **B, int size, int dx, int dy);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send k");
        return EXIT_SUCCESS;
    }
    int k = atoi(argv[1]);
    int **A, **B, **C;
    int rows, cols;
    int shm_A_id, shm_B_id, shm_C_id;

    rows = cols = k;

    pid_t root = getpid();
    int n_children, child_id;

    create_shm(&shm_A_id, (void **)&A, rows, cols, sizeof(int));
    create_shm(&shm_B_id, (void **)&B, rows, cols, sizeof(int));
    create_shm(&shm_C_id, (void **)&C, rows, cols, sizeof(int));

    n_children = (k % 2 == 0) ?  k/2 : k/2+1;
    printf("N_children: %d\n", n_children);

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //padre
        int n1, n2;
        n1 = 2;
        n2 = 3;

        for(int i=0; i < rows; i++){
            for(int j=0; j < cols; j++){
                A[i][j] = pow(n1, i);
                B[i][j] = pow(n2, i);
            }
        } 

        for(int i=0; i < n_children; i++) wait(NULL);

        printf("\n");
        show_matrix(C, rows, cols);
        
        shmdt(A);
        shmdt(B);
        shmdt(C);
        shmctl(shm_A_id, IPC_RMID, NULL);
        shmctl(shm_B_id, IPC_RMID, NULL);
        shmctl(shm_C_id, IPC_RMID, NULL);
    }else{ //hijo
        sleep(child_id+2);

        printf("Child %d\n", child_id);
        
        //filas
        for(int i=child_id; i < k-child_id; i++){
            //printf("rows [%d][%d]\n", child_id, i);
            //printf("rows [%d][%d]\n", k-child_id-1, i);
            C[child_id][i] = mult_matx(A, B, k, child_id, i);//fila superior
            C[k-child_id-1][i] = mult_matx(A, B, k, k-child_id-1, i); //fila inferior

        }

        printf("\n");
        //columnas
        for(int j=child_id+1; j < k-child_id-1; j++){
            //printf("cols [%d][%d]\n", j, child_id);
            //printf("cols [%d][%d]\n", j, k-child_id-1);
            C[j][child_id] =  mult_matx(A, B, k, j, child_id);
            C[j][k-child_id-1] =  mult_matx(A, B, k, j, k-child_id-1);
        }

        shmdt(A);
        shmdt(B);
        shmdt(C);
    }

    return EXIT_SUCCESS;
}

int mult_matx(int **A, int **B, int size, int dx, int dy){
    int value = 0;
    for(int i=0; i < size; i++){
        value += A[dx][i] * B[i][dy];
    }
    return value;
}

void show_matrix(int **mtx, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("[%d]\t ", mtx[i][j]);
        }
        printf("\n");
    }

    printf("\n");
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