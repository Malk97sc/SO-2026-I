//P->H1->H2->...->N-1->P

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LEN 128

void showtree();

int main(int argc, char **argv){
    pid_t root = getpid();
    int n_children = 2, child_id, level = 1;
    int n_pipes = 8;
    int end = 0;
    char buffer[MAX_LEN];

    int pipes[n_pipes][2];

    for(int i=0; i < n_pipes; i++){
        pipe(pipes[i]);
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()){
            if(!fork()){
                level = 2;
            }
            break;
        } 
    }

    if(root == getpid()){ //Parent Process
        showtree();
        char result[MAX_LEN], end_str[MAX_LEN];

        for(int i=0; i < n_pipes; i++){
            if(i == 0 || i == 2){ //donde recibe
                close(pipes[i][1]);
            }else if(i == 1 || i == 3){ //donde envia
                close(pipes[i][0]);
            }else{
                close(pipes[i][0]); close(pipes[i][1]);
            }
        }

        while(!end){
            printf("Send msg: \n");
            if(fgets(buffer, sizeof(buffer), stdin) == NULL){
                perror("fail fgets");
                exit(1);
            }
            buffer[strcspn(buffer, "\n")] = '\0';

            if(strncmp(buffer, "Fin", 3) == 0){
                end = 1;
            }

            write(pipes[3][1], buffer, MAX_LEN);
            
            read(pipes[2][0], result, MAX_LEN);
            printf("[PARENT] Result String: %s\n", result);

            write(pipes[1][1], result, MAX_LEN);

            read(pipes[0][0], end_str, MAX_LEN);

            printf("[PARENT] End String: %s\n", end_str);
        }

        close(pipes[3][1]);
        close(pipes[2][0]);
        close(pipes[1][1]);
        close(pipes[0][0]);

        for(int i=0; i < n_children; i++) wait(NULL);

    }else{ //Child Process
        sleep(1);
        char result[MAX_LEN], end_str[MAX_LEN];

        if(level == 1){
            if(child_id == 0){
                for(int i=0; i < n_pipes; i++){
                    if(i == 1 || i == 4){ //leemos
                        close(pipes[i][1]);
                    }else if(i == 5 || i == 0){ //escribimos
                        close(pipes[i][0]);
                    }else{
                        close(pipes[i][0]);
                        close(pipes[i][1]);
                    }
                }

                while(read(pipes[1][0], buffer, MAX_LEN) > 0){
                    printf("PID[%d] Msg is: %s\n", getpid(), buffer);

                    write(pipes[5][1], buffer, MAX_LEN);
                    
                    read(pipes[4][0], result, MAX_LEN);
                    printf("PID[%d] Msg is: %s\n", getpid(), result);
                    
                    write(pipes[0][1], result, MAX_LEN);
                }
                close(pipes[1][0]);
                close(pipes[5][1]);
                close(pipes[4][0]);
                close(pipes[0][1]);

            }else{
                for(int i=0; i < n_pipes; i++){
                    if(i == 3 || i == 6){ //leemos
                        close(pipes[i][1]);
                    }else if(i == 2 || i == 7){
                        close(pipes[i][0]);
                    }else{
                        close(pipes[i][0]);
                        close(pipes[i][1]);
                    }
                }

                while(read(pipes[3][0], buffer, MAX_LEN) > 0){
                    printf("PID[%d] Msg is: %s\n", getpid(), buffer);

                    write(pipes[7][1], buffer, MAX_LEN);
                    
                    read(pipes[6][0], result, MAX_LEN);
                    printf("PID[%d] Msg is: %s\n", getpid(), result);

                    write(pipes[2][1], result, MAX_LEN);
                }

                close(pipes[3][0]);
                close(pipes[7][1]);
                close(pipes[6][0]);
                close(pipes[2][1]);
            }
        }else{
            if(child_id == 0){
                for(int i=0; i < n_pipes; i++){
                    if(i == 4){
                        close(pipes[i][0]);
                    }else if(i == 5){
                        close(pipes[i][1]);
                    }else{
                        close(pipes[i][0]);
                        close(pipes[i][1]);
                    }
                }

                while(read(pipes[5][0], buffer, MAX_LEN) > 0){
                    printf("PID[%d] Msg is: %s\n", getpid(), buffer);

                    write(pipes[4][1], buffer, MAX_LEN);
                }

                close(pipes[5][0]);
                close(pipes[4][1]);

            }else{
                for(int i=0; i < n_pipes; i++){
                    if(i == 6){
                        close(pipes[i][0]);
                    }else if(i == 7){
                        close(pipes[i][1]);
                    }else{
                        close(pipes[i][0]);
                        close(pipes[i][1]);
                    }
                }

                while(read(pipes[7][0], buffer, MAX_LEN) > 0){
                    printf("PID[%d] Msg is: %s\n", getpid(), buffer);

                    write(pipes[6][1], buffer, MAX_LEN);
                }
                
                close(pipes[7][0]);
                close(pipes[6][1]);
            }
        }
        
    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}