#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <string.h>

#define MAX_USERS 100001
#define MAX_VIDEOS 50001

void showtree();

typedef struct {
    int user_id;
    int video_id;
    int aspect;
    int value;
} Report;

typedef struct {
    int user_id;
    int video_id;
    int face_score;
    int audio_score;
    int sync_score;

    int has_face, has_audio, has_sync;
} Video;

typedef struct {
    Video videos[MAX_VIDEOS];

    int write_pos;
    int read_pos;
    int count;

    int B;

    int finished;
} SharedVideos;

typedef struct {
    int suspicious_videos[MAX_USERS];
    int total_suspicious;
    int finished;
} SharedCounts;

typedef struct {
    int user_id;
    int count;
} Result;

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file");
        return EXIT_SUCCESS;
    }
    pid_t root = getpid();
    int n_childern = 3, child_id;
    int n_pipes = 2;
    
    int pipes[n_pipes][2];
    SharedVideos *shared_videos;
    SharedCounts *shared_counts;
    int videos_shm_id, counts_shm_id;

    for(int i=0; i < n_pipes; i++){
        pipe(pipes[i]);
    }

    //printf("")

    videos_shm_id = shmget(IPC_PRIVATE, sizeof(SharedVideos), IPC_CREAT | 0666);
    shared_videos = (SharedVideos*)shmat(videos_shm_id, NULL, 0);

    counts_shm_id = shmget(IPC_PRIVATE, sizeof(SharedCounts), IPC_CREAT | 0666);
    shared_counts = (SharedCounts*)shmat(counts_shm_id, NULL, 0);

    memset(shared_videos, 0, sizeof(SharedVideos));
    memset(shared_counts, 0, sizeof(SharedCounts));


    for(child_id = 0; child_id < n_childern; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){
        showtree();
        close(pipes[0][0]);
        close(pipes[1][1]);

        int B, N;
        int total_ban, total_susp;
        Report rep;
        Result result;

        FILE *fl = fopen(argv[1], "r");
        if(!fl) exit(1);

        fscanf(fl, "%d %d", &B, &N);
        printf("B: %d, N: %d\n", B, N);

        total_ban = total_susp = 0;
        shared_videos->B = B;

        for(int i=0; i < N; i++){
            fscanf(fl, "%d %d %d %d", &rep.user_id, &rep.video_id, &rep.aspect, &rep.value);

            write(pipes[0][1], &rep, sizeof(Report));
        }
        rep.user_id = -1;
        write(pipes[0][1], &rep, sizeof(Report));
        close(pipes[0][1]);

        fclose(fl);

        while(read(pipes[1][0], &result, sizeof(Result)) > 0){
            if(result.user_id == -1){
                total_susp = result.count;
                break;
            }

            printf("%d %d\n", result.user_id, result.count);
            total_ban++;
        }

        printf("All banned: %d\n", total_ban);
        printf("All suspisous: %d\n", total_susp);

        for(int i=0; i < n_childern; i++) wait(NULL);
        shmdt(shared_videos);
        shmctl(videos_shm_id, IPC_RMID, NULL);
        shmdt(shared_counts);
        shmctl(counts_shm_id, IPC_RMID, NULL);
    }else{

        switch (child_id){
            case 0:{
                close(pipes[0][1]);
                close(pipes[1][0]); close(pipes[1][1]);
                
                Report rep;
                Video all_videos[MAX_VIDEOS], *tmp;

                while(read(pipes[0][0], &rep, sizeof(Report)) > 0){
                    //printf("User: %d\n", rep.user_id);
                    if(rep.user_id == -1) break;

                    tmp = &all_videos[rep.video_id];

                    tmp->video_id = rep.video_id;
                    tmp->user_id = rep.user_id;

                    switch (rep.aspect){
                        case 1:{
                            tmp->has_face = 1;
                            tmp->face_score = rep.value;
                            break;
                        }
                        case 2:{
                            tmp->has_audio = 1;
                            tmp->audio_score = rep.value;
                            break;
                        }
                        case 3:{
                            tmp->has_sync = 1;
                            tmp->sync_score = rep.value;
                            break;
                        }
                    }

                    if(tmp->has_face && tmp->has_audio && tmp->has_sync){
                        //printf("shm_ count: %d and B: %d\n", shared_videos->count, shared_videos->B);
                        while(shared_videos->count == shared_videos->B){
                            usleep(2000);
                        }

                        shared_videos->videos[shared_videos->write_pos] = *tmp;

                        shared_videos->write_pos = (shared_videos->write_pos + 1) % shared_videos->B;
                        shared_videos->count++;
                        //printf("sending user to shm: %d\n", rep.user_id);
                    }
                }

                shared_videos->finished = 1; //fin de todos los videos

                close(pipes[0][0]);
                break;
            }
            case 1:{
                close(pipes[0][0]); close(pipes[0][1]);
                close(pipes[1][0]); close(pipes[1][1]);

                Video temp;
                int count = 0;

                while(1){
                    while(shared_videos->count == 0){
                        usleep(2000);
                        if(shared_videos->finished) break;
                    }

                    if(shared_videos->finished) break;

                    temp = shared_videos->videos[shared_videos->read_pos];

                    shared_videos->read_pos = (shared_videos->read_pos + 1) % shared_videos->B;
                    shared_videos->count--;
                    count = 0;

                    if(temp.face_score >= 80) count++;
                    if(temp.audio_score >= 75) count++;
                    if(temp.sync_score <= 35) count++;

                    if(count >= 2){
                        shared_counts->suspicious_videos[temp.user_id]++;
                        shared_counts->total_suspicious++;
                    }
                }
                
                shared_counts->finished = 1;
                
                break;
            }
            case 2:{
                close(pipes[0][0]); close(pipes[0][1]);
                close(pipes[1][0]); 

                while(!shared_counts->finished) usleep(2000);
                
                Result result;

                for(int i=0; i < MAX_USERS; i++){
                    if(shared_counts->suspicious_videos[i] > 3){
                        result.user_id = i;
                        result.count = shared_counts->suspicious_videos[i];

                        write(pipes[1][1], &result, sizeof(Result));
                    }
                }

                result.user_id = -1;
                result.count = shared_counts->total_suspicious;

                write(pipes[1][1], &result, sizeof(Result));                
                close(pipes[1][1]);

                break;
            }
        }

        shmdt(shared_counts);
        shmdt(shared_videos);
    }
    

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}