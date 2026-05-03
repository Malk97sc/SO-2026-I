#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define MAX_CHILDREN 4

void showtree();

typedef struct {
    int done[MAX_CHILDREN]; //[0] [1] [2] [3]
    int lock; 
    int ready; //0 = no esta ready, 1 esta ready
} SharedData;

unsigned int sizeof_dm(int rows, int cols, size_t size_elements);
void create_index(void **matrix, int rows, int cols, size_t size_elements);
void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements);

void copy_matrix(int **A, int **B, int rows, int cols);
void show_matrix(int **A, int rows, int cols);
void read_file(const char *file, int ***mtx, int *rows, int *cols, int *n_days);

int num_neighbors(int **mtx, int dx, int dy, int rows, int cols, int child_id);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file");
        return EXIT_SUCCESS;
    }
    pid_t root = getpid();
    int n_children = MAX_CHILDREN, child_id;

    int **data, **results, **temp;
    int n_days;
    int rows, cols;
    int shm_id, rst_shm_id, shared_shm_id;

    SharedData *shared;

    read_file(argv[1], &temp, &rows, &cols, &n_days);

    shared_shm_id = shmget(IPC_PRIVATE, sizeof(SharedData), 0666 | IPC_CREAT);
    shared = (SharedData*) shmat(shared_shm_id, NULL, 0);
    create_shm(&shm_id, (void **)&data, rows, cols, sizeof(int));
    create_shm(&rst_shm_id, (void **)&results, rows, cols, sizeof(int));

    copy_matrix(data, temp, rows, cols);
    for(int i=0; i < rows; i++){
        free(temp[i]);
    }
    free(temp);

    shared->lock = 0;
    shared->ready = 0;

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){
        showtree();
        copy_matrix(results, data, rows, cols); //inicializo la results

        for(int d=0; d < n_days; d++){
            while(shared->ready == 1){
                usleep(50000);
            }

            //ningun hijo esta trabajando
            shared->done[0] = 0;
            shared->done[1] = 0;
            shared->done[2] = 0;
            shared->done[3] = 0;

            printf("----- Day: %d -----\n", d);
            copy_matrix(data, results, rows, cols);
            show_matrix(data, rows, cols);

            shared->ready = 1;
        }

        for(int i=0; i < n_children; i++) wait(NULL);

        shmdt(data);
        shmctl(shm_id, IPC_RMID, NULL);
        shmdt(results);
        shmctl(rst_shm_id, IPC_RMID, NULL);
    }else{
        int count_end = 0, end_round = 0, neig = 0;

        for(int d=0; d < n_days; d++){
            neig = count_end = 0;
            while(!shared->ready || end_round){
                end_round = 0;
                //printf("wait ready: %d\n", child_id);
                usleep(50000);
            }
            
            while(shared->lock) usleep(50000);
            shared->lock = 1;

            //printf("Child: %d\n", child_id);
            //show_matrix(data, rows, cols);

            for(int i=0; i < rows; i++){
                for(int j=0; j < cols; j++){
                    switch(child_id){
                        case 0:{
                            if(data[i][j] == 0){ //baja2media
                                neig = num_neighbors(data, i, j, rows, cols, child_id);

                                if(neig >= 4){
                                    results[i][j] = 1;
                                }
                            }
                            break;
                        }
                        case 1:{
                            if(data[i][j] == 1){ //media2baja
                                neig = num_neighbors(data, i, j, rows, cols, child_id);

                                float prob = 0.1 + (neig * 0.03);
                                if(rand() < prob){
                                    results[i][j] = 0;
                                }
                            }
                            break;
                        }
                        case 2:{
                            if(data[i][j] == 1){ //media2alta
                                neig = num_neighbors(data, i, j, rows, cols, child_id);

                                if(neig >= 3){
                                    results[i][j] = 2;
                                }
                            }
                            break;
                        }
                        case 3:{
                            if(data[i][j] == 2){ //alta2media
                                neig = num_neighbors(data, i, j, rows, cols, child_id);

                                float prob = 0.15 + (neig * 0.05);
                                if(rand() < prob){
                                    results[i][j] = 1;
                                }
                            }
                            break;
                        }
                    }
                }
            }               

            shared->done[child_id] = 1;
            end_round = 1;
            shared->lock = 0;            

            for(int i=0; i < n_children; i++){
                if(shared->done[i] == 1){
                    count_end++;
                }
            }

            if(count_end == n_children){
                shared->ready = 0;  
            }                      
        }
        
        shmdt(results);
        shmdt(data);
    }   

    
    return EXIT_SUCCESS;
}

int num_neighbors(int **mtx, int dx, int dy, int rows, int cols, int child_id){
    int count = 0;
    for(int i=dx-1; i <= dx+1; i++){
        for(int j=dy-1; j <= dy+1; j++){
            if(i == dx && j == dy) continue;

            if(i >= 0 && i < rows && j >= 0 && j < cols){
                switch(child_id){
                    case 0:{
                        if(mtx[i][j] == 1 || mtx[i][j] == 2){ //baja2media
                            count++;
                        }
                        break;
                    }
                    case 1:{
                        if(mtx[i][j] == 0){ //media2baja
                            count++;
                        }
                        break;
                    }
                    case 2:{
                        if(mtx[i][j] == 2){ //media2alta
                            count++;
                        }
                        break;
                    }
                    case 3:{
                        if(mtx[i][j] == 0){ //alta2media
                            count++;
                        }
                        break;
                    }
                }
            }
        }
    }

    return count;
}

void show_matrix(int **A, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
}

void read_file(const char *file, int ***mtx, int *rows, int *cols, int *n_days){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail fopen\n");
        exit(1);
    }

    fscanf(fl, "%d", n_days);
    fscanf(fl, "%d", rows);
    fscanf(fl, "%d", cols);
    printf("n_days %d. Rows: %d - Cols: %d\n", *n_days, *rows, *cols);

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

    fclose(fl);
}

void copy_matrix(int **A, int **B, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            A[i][j] = B[i][j];
        }
    }
}

void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements){
    size_t size_mtx;

    size_mtx = sizeof_dm(rows, cols, size_elements);
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