#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define MAX 10000

typedef struct {
    int video_id;
    int user_id;
    int face_score, audio_score, sync_score;
    int origin;

    int partial_risk;
    int is_suspisous; //0 = n0, 1 = si es sospechoso
} Data;

typedef struct {
    int user_id;
    int suspisous[MAX];
} Result;

void showtree();

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send the file\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = 3, child_id;
    int n_pipes = n_children + 1;

    int pipes[n_pipes][2];

    for(int i=0; i < n_pipes; i++){
        if(pipe(pipes[i]) == -1){
            perror("Fail pipe");
            exit(1);
        }
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //Parent Process
        showtree();

        int N, B;
        int total_susp, total_bann;
        Data data, recived;
        Result results = {0};
        FILE *fl;

        total_susp = total_bann = 0;

        for(int i=0; i < n_pipes; i++){
            if(i != 0){
                close(pipes[i][1]); 
            }
            
            if(i != n_pipes-1){
                close(pipes[i][0]);
            }
        }

        fl = fopen(argv[1], "r");
        if(!fl) exit(1);

        fscanf(fl, "%d %d", &B, &N);
        printf("B: %d, N: %d\n", B, N);

        int total = (N + B-1) / B;

        for(int i=0; i < total; i++){
            for(int j=0; j < B; j++){ //mando datos
                fscanf(fl, "%d %d %d %d %d %d", &data.video_id,
                    &data.user_id, &data.face_score, &data.audio_score,
                    &data.sync_score, &data.origin
                );

                data.partial_risk = 0;
                data.is_suspisous = 0;
                
                write(pipes[0][1], &data, sizeof(Data));
            }

            for(int j=0; j < B; j++){ //leo datos
                read(pipes[n_pipes-1][0], &recived, sizeof(Data));

                if(recived.is_suspisous && recived.user_id >= 0 && recived.user_id < MAX){
                    results.suspisous[recived.user_id]++;
                    total_susp++;
                }
            }
        }

        data.video_id = -1; //-1 representa el final, ya no hay q leer mas
        write(pipes[0][1], &data, sizeof(data));
        read(pipes[n_pipes-1][0], &recived, sizeof(Data));

        close(pipes[0][1]);
        close(pipes[n_pipes-1][0]);

        for(int i=0; i < n_children; i++) wait(NULL);

        for(int i=0; i < MAX; i++){
            if(results.suspisous[i] > 3){
                printf("User: %d, Count: %d\n", i, results.suspisous[i]);
                total_bann++;
            }
        }

        printf("All banned: %d\n", total_bann);
        printf("All suspisous: %d\n", total_susp);

        fclose(fl);

    }else{ //Child Process
        for(int i=0; i < n_pipes; i++){
            if(i != child_id){
                close(pipes[i][0]);
            }
            
            if(i != child_id + 1){
                close(pipes[i][1]); 
            }
        }

        Data recived;

        while(read(pipes[child_id][0], &recived, sizeof(Data)) > 0){
            switch(child_id){
                case 0:{
                    if(recived.face_score >= 80){
                        recived.partial_risk++;
                    }
                    break;
                }
                case 1:{
                    if(recived.audio_score >= 75){
                        recived.partial_risk++;
                    }
                    break;
                }
                case 2:{
                    if(recived.sync_score <= 35){
                        recived.partial_risk++;
                    }

                    if(recived.partial_risk >= 2){
                        recived.is_suspisous = 1;
                    }
                    
                    break;
                }
            }

            if(recived.video_id == -1){
                write(pipes[child_id+1][1], &recived, sizeof(Data));
                close(pipes[child_id][0]);
                close(pipes[child_id+1][1]);
                break;
            }

            write(pipes[child_id+1][1], &recived, sizeof(Data));
        }
        
    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}