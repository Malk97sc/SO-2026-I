#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_LEN 64
#define MAX_RESULTS 128

typedef struct {
    int min_total;
    char region[MAX_LEN];
} Result;

typedef struct {
    char turno;
    char vm[MAX_LEN];
    char region[MAX_LEN];
    char fail[MAX_LEN];
    int minutes;
    char cause[MAX_LEN];
} FileData;

typedef struct {
    Result results[MAX_RESULTS];
    int n_results;
    int busy; //0 = no esta busy, 1 esta busy
} SharedData;

void showtree();

void read_file(const char *file, char ***vec, int *size);

void create_shmV(int *x_shm, void **vec, size_t size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file");
        return EXIT_SUCCESS;
    }
    pid_t root = getpid();
    int n_children, child_id;

    char **vec;
    int size;
    int pos = 0;
    int chunk = -1;

    size_t total_chars = 0;
    
    SharedData *shared;
    char *data; //string
    int *mov; //desplazamiento
    int data_shm_id, mov_shm_id, result_shm_id;

    read_file(argv[1], &vec, &size);

    for(int i=0; i < size; i++){
        total_chars += strlen(vec[i]) + 1;
    }

    create_shmV(&data_shm_id, (void**)&data, total_chars * sizeof(char));
    create_shmV(&mov_shm_id, (void**)&mov, size * sizeof(int));
    result_shm_id = shmget(IPC_PRIVATE, sizeof(SharedData), 0666 | IPC_CREAT);
    shared = (SharedData*) shmat(result_shm_id, NULL, 0);
    
    shared->n_results = 0;

    for(int i=0; i < size; i++){
        mov[i] = pos;
        strcpy(data + pos, vec[i]);
        pos += strlen(vec[i]) + 1;
    }

    for(int i=0; i < size; i++){
        free(vec[i]);
    }
    free(vec);
    n_children = size;

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){
        showtree();

        for(int i=0; i < n_children; i++) wait(NULL);

        int max = -1;
        char reg[MAX_LEN];

        for(int i=0; i < shared->n_results; i++){
            printf("Region: %s, (%d minutes)\n", shared->results[i].region, shared->results[i].min_total);    
            if(shared->results[i].min_total > max){
                max = shared->results[i].min_total;
                strcpy(reg, shared->results[i].region);
            }
        }

        printf("Region with longest downtime: %s, (%d minutes)\n", reg, max);

        shmdt(shared);
        shmdt(data);
        shmdt(mov);
        shmctl(result_shm_id, IPC_RMID, NULL);
        shmctl(data_shm_id, IPC_RMID, NULL);
        shmctl(mov_shm_id, IPC_RMID, NULL);
    }else{
        FILE *fl = fopen(data + mov[child_id], "r");
        
        int size_in, total_min = 0;
        FileData fl_data;
        Result result;

        fscanf(fl, "%d", &size_in);
        //printf("\nChild_id %d LOG: %s Size log: %d\n", child_id, data + mov[child_id] ,size_in);

        for(int i=0; i < size_in; i++){
            fscanf(fl, " %c;%[^;];%[^;];%[^;];%d;%s", &fl_data.turno,
                fl_data.vm, fl_data.region, fl_data.fail, &fl_data.minutes, fl_data.cause
            );

            /*printf("%c;%s;%s;%s;%d;%s\n", fl_data.turno,
                fl_data.vm, fl_data.region, fl_data.fail, fl_data.minutes, fl_data.cause
            );*/

            total_min += fl_data.minutes;
        }
        printf("Region: %s, Total min: %d\n", fl_data.region, total_min);

        while(shared->busy) usleep(50000);
        shared->busy = 1;
        
        strcpy(shared->results[shared->n_results].region, fl_data.region);
        shared->results[shared->n_results].min_total = total_min;

        shared->n_results++;

        shared->busy = 0;

        shmdt(shared);
        shmdt(data);
        shmdt(mov);
        fclose(fl);
    }   

    
    return EXIT_SUCCESS;
}

void create_shmV(int *x_shm, void **vec, size_t size){
    *x_shm = shmget(IPC_PRIVATE, size, 0666 | IPC_CREAT);
    if(*x_shm == -1){
        exit(1);
    }

    *vec = shmat(*x_shm, NULL, 0);
    if(*vec == (void*)-1){
        exit(1);
    }
}

void show_matrix(int **A, int rows, int cols){
    for(int i=0; i < rows; i++){
        for(int j=0; j < cols; j++){
            printf("%d ", A[i][j]);
        }
        printf("\n");
    }
}

void read_file(const char *file, char ***vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    char buffer[MAX_LEN];

    fscanf(fl, "%d", size);
    fgetc(fl);
    printf("Size: %d\n", *size);

    *vec = (char**) malloc(*size * sizeof(char *));
    if(!(*vec)){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        fgets(buffer, sizeof(buffer), fl);
        buffer[strcspn(buffer, "\n")] = '\0';
        (*vec)[i] = (char*) malloc((strlen(buffer) + 1) * sizeof(char));
        strcpy((*vec)[i], buffer);
    }

    for(int i=0; i < *size; i++){
        printf("%s: and len %ld\n", (*vec)[i], strlen((*vec)[i]));
    }

    fclose(fl);
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}