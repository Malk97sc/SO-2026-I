#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_USERS 10000

typedef struct {
    int video_id;
    int user_id;
    int face_score, audio_score, sync_score;
    int origin;

    int face_risk;
    int audio_risk;
    int sync_risk;
    int count_suspisous;
    int is_suspisous; //0 = n0, 1 = si es sospechoso
} Result;

typedef struct {
    int done[3];
    int end;
    int block_num;
} SharedData;

void showtree();

unsigned int sizeof_dm(int rows, int cols, size_t size_elements);
void create_index(void **matrix, int rows, int cols, size_t size_elements);
void create_shm(int *shm_id, void **mtx, int rows, int cols, size_t size_elements);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file");
        return EXIT_SUCCESS;
    }
    pid_t root = getpid();
    int n_children = 3, child_id;

    int **mtx;
    int cols = 6;
    int B, N;
    int shm_id, shared_shm_id, results_shm_id;

    SharedData *shared;
    Result *results;

    FILE *fl = fopen(argv[1], "r");

    fscanf(fl, "%d %d", &B, &N);
    fclose(fl);

    create_shm(&shm_id, (void **)&mtx, B, cols, sizeof(int));
    shared_shm_id = shmget(IPC_PRIVATE, sizeof(SharedData), 0666 | IPC_CREAT);
    results_shm_id = shmget(IPC_PRIVATE, MAX_USERS * sizeof(Result), 0666 | IPC_CREAT);

    shared = (SharedData*) shmat(shared_shm_id, NULL, 0);
    results = (Result*) shmat(results_shm_id, NULL, 0);

    memset(shared, 0, sizeof(SharedData));
    memset(results, 0, sizeof(Result));

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //padre
        showtree();

        FILE *fl = fopen(argv[1], "r");

        fscanf(fl, "%d %d", &B, &N);
        printf("B: %d, N: %d\n", B, N);
        int total_susp, total_bann;
        
        shared->end = 0;
        shared->block_num = 0;
        total_susp = total_bann = 0;

        for(int i=0; i < N/B; i++){
            for(int j=0; j < B; j++){
                fscanf(fl, "%d %d %d %d %d %d", &mtx[j][0],
                    &mtx[j][1], &mtx[j][2], &mtx[j][3],
                    &mtx[j][4], &mtx[j][5]
                );
            }
            //printf("\nBlock: %d\n", shared->block_num);

            //ningun hijo esta trabajando
            shared->done[0] = 0;
            shared->done[1] = 0;
            shared->done[2] = 0;

            shared->block_num++;
            while(!shared->done[0] || !shared->done[1] || !shared->done[2]){
                //printf("Parent sleeping\n");
                usleep(5000);
            }

            for(int j=0; j < B; j++){
                if((results[mtx[j][1]].face_risk + 
                    results[mtx[j][1]].audio_risk + 
                    results[mtx[j][1]].sync_risk) >= 2){
                    
                    results[mtx[j][1]].count_suspisous++;
                    results[mtx[j][1]].is_suspisous = 1;
                    total_susp++;
                }
            }
        }

        shared->end = 1;

        for(int i=0; i < MAX_USERS; i++){
            if(results[i].count_suspisous > 3){
                printf("User %d, Count: %d\n", i, results[i].count_suspisous);
                total_bann++;
            }
        }

        for(int i=0; i < n_children; i++) wait(NULL);

        printf("All banned: %d\n", total_bann);
        printf("All suspisous: %d\n", total_susp);

        fclose(fl);
        shmdt(mtx);
        shmdt(shared);
        shmctl(shared_shm_id, IPC_RMID, NULL);
        shmctl(shm_id, IPC_RMID, NULL); 
    }else{ //hijos
        int last_block = 0;
        printf("Child %d\n", child_id);

        while(!shared->end){
            while(last_block == shared->block_num){
                if(shared->end){ //1
                    shmdt(shared);
                    shmdt(mtx);
                    exit(1);
                }
                usleep(2000);  
            }
            last_block = shared->block_num;

            /*for(int i=0; i < B; i++){
                printf("Child_id: %d ", child_id);
                for(int j=0; j < cols; j++){
                    printf("[%d] ", mtx[i][j]);
                }
                printf("\n");
            }*/

            for(int i=0; i < B; i++){
                switch(child_id){
                    case 0:{
                        results[mtx[i][1]].face_risk = (mtx[i][2] >= 80);
                        break;
                    }
                    case 1:{
                        results[mtx[i][1]].audio_risk = (mtx[i][3] >= 75);
                        break;
                    }
                    case 2:{                        
                        results[mtx[i][1]].sync_risk = (mtx[i][4] <= 35);
                        break;
                    }
                }
            }

            shared->done[child_id] = 1;
        }


        shmdt(shared);
        shmdt(mtx);
    }


    
    return EXIT_SUCCESS;
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