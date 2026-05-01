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
    int count;
} Result;

void showtree();

int is_suspisous(Data data);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send the file\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = 3, child_id;
    int n_pipes = n_children;

    int in_pipes[n_children][2];
    int out_pipes[n_children][2];

    for(int i=0; i < n_pipes; i++){
        pipe(in_pipes[i]);
        pipe(out_pipes[i]);
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //Parent Process
        showtree();

        for(int i=0; i < n_pipes; i++){
            close(in_pipes[i][0]);
            close(out_pipes[i][1]);
        }

        int N, B, turn = 0;
        int total_susp, total_bann;
        Data data, recived;
        Result results = {0}, total[MAX];
        FILE *fl;

        total_susp = total_bann = 0;

        fl = fopen(argv[1], "r");
        if(!fl) exit(1);

        fscanf(fl, "%d %d", &B, &N);
        printf("B: %d, N: %d\n", B, N);

        for(int i=0; i < n_pipes; i++){
            write(in_pipes[i][1], &B, sizeof(int));
        }

        for(int i=0; i < N/B; i++){
            for(int j=0; j < B; j++){
                fscanf(fl, "%d %d %d %d %d %d", &data.video_id,
                    &data.user_id, &data.face_score, &data.audio_score,
                    &data.sync_score, &data.origin
                );
                write(in_pipes[turn][1], &data, sizeof(Data));
                turn++;

                if(turn == n_children){
                    turn = 0;
                }
            }
            //printf("\nRound: %d\n", i);
        }

        for(int i=0; i < n_pipes; i++){
            close(in_pipes[i][1]);
        }
        memset(total, 0, sizeof(total));
        
        for(int i=0; i < n_pipes; i++){
            while(read(out_pipes[i][0], &results, sizeof(results)) > 0){
                if(results.user_id == -1) break;

                total[results.user_id].user_id = results.user_id;
                total[results.user_id].count += results.count;
            }
        }

        for(int i=0; i < n_pipes; i++){
            close(out_pipes[i][0]);
        }

        fclose(fl);

        for(int i=0; i < MAX; i++){
            if(total[i].count > 0){
                total_susp += total[i].count;

                if(total[i].count > 3){
                    printf("User: %d, Count: %d\n", i,  total[i].count);
                    total_bann++;
                }
            }
        }

        for(int i=0; i < n_children; i++) wait(NULL);

        printf("All banned: %d\n", total_bann);
        printf("All suspisous: %d\n", total_susp);

    }else{ //Child Process
        for(int i=0; i < n_pipes; i++){
            if(i == child_id){
                close(in_pipes[i][1]);
                close(out_pipes[i][0]);
            }else{
                close(in_pipes[i][0]); close(in_pipes[i][1]);
                close(out_pipes[i][0]); close(out_pipes[i][1]);
            }
        }
        int B, lines = 0;
        Data recived;
        Result resp[MAX];

        memset(resp, 0, sizeof(resp));

        read(in_pipes[child_id][0], &B, sizeof(int)); //B
        
        while(read(in_pipes[child_id][0], &recived, sizeof(Data)) > 0){
            //printf("Child %d and video_id is: %d\n", child_id, recived.video_id);

            if(is_suspisous(recived)){
                resp[recived.user_id].user_id = recived.user_id;
                resp[recived.user_id].count++;
            }

            lines++;

            if(lines == B){
                for(int i=0; i < MAX; i++){
                    if(resp[i].count > 0){
                        write(out_pipes[child_id][1], &resp[i], sizeof(Result));
                    }
                }
                lines = 0;
                memset(resp, 0, sizeof(resp));
            }
        }
        close(in_pipes[child_id][0]);
        
        for(int i=0; i < MAX; i++){
            if(resp[i].count > 0){
                write(out_pipes[child_id][1], &resp[i], sizeof(Result));
            }
        }
        Result end = {-1, -1};
        write(out_pipes[child_id][1], &end, sizeof(Result));

        close(out_pipes[child_id][1]);
    }

    return EXIT_SUCCESS;
}

int is_suspisous(Data data){
    int is = 0;
    if(data.face_score >= 80) is++;
    if(data.audio_score >= 75) is++;
    if(data.sync_score <= 35) is++;
    return is >= 2;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}