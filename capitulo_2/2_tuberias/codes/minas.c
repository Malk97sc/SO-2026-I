#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>

#define MAX_MINES 100

typedef struct {
    int dx[MAX_MINES], dy[MAX_MINES];
    int count;
    pid_t child;
} Map;

void showtree();

void read_file(const char *file, int ***mtx, int *rows, int *cols);
void is_mine(int **mtx, int dx, int dy, int rows, int cols, Map *map);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send a file\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int rows, cols;
    int chunk;

    int **mtx;
    int n_children = 4, child_id;

    int n_pipes = 4;
    int pipes[n_pipes][2];

    read_file(argv[1], &mtx, &rows, &cols);

    for(int i=0; i < n_pipes; i++){
        pipe(pipes[i]);
    }

    chunk = rows / n_children;

    for(child_id=0 ; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //padre
        //showtree();
        for(int i=0; i < n_pipes; i++) close(pipes[i][1]);
        for(int i=0; i < n_children; i++) wait(NULL);

        Map results;
        for(int i=0; i < n_children; i++){
            read(pipes[i][0], &results, sizeof(Map));
            printf("Child [%d] found a mine in the position: ", results.child);
            for(int j=0; j < results.count; j++){
                printf("[%d][%d] ", results.dx[j], results.dy[j]);
            }
            printf("\n");
        }

        for(int i=0; i < n_pipes; i++) close(pipes[i][0]);
    }else{ //hijos
        for(int i=0; i < n_pipes; i++) close(pipes[i][0]);
        int idx_start, idx_end;
        Map map;

        map.count = 0;
        map.child = child_id;
        idx_start = chunk * child_id;
        idx_end = (child_id == n_children-1) ? rows : chunk * (child_id+1);

        //printf("Child: %d. idx_start: %d, idx_end: %d\n", child_id, idx_start, idx_end);

        for(int i=idx_start; i < idx_end; i++){
            for(int j=0; j < cols; j++){
                if(mtx[i][j] == 1){ //posible mina
                    is_mine(mtx, i, j, rows, cols, &map);
                }
            }
        }

        write(pipes[child_id][1], &map, sizeof(map));
        for(int i=0; i < n_pipes; i++) close(pipes[i][1]);
    }

    for(int i=0; i < rows; i++){
        free(mtx[i]);
    }
    free(mtx);
    return EXIT_SUCCESS;
}

void is_mine(int **mtx, int dx, int dy, int rows, int cols, Map *map){
    for(int i=dx-1; i <= dx+1; i++){
        for(int j=dy-1; j <= dy+1; j++){
            if(i == dx && j == dy) continue;

            if(i >= 0 && i < rows && j >= 0 && j < cols){
                if(mtx[i][j] == 2){ //es una mina
                    map->dx[map->count] = dx;
                    map->dy[map->count] = dy;
                    map->count++;
                    return;
                }
            }
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

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}