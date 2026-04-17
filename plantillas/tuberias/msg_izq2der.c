//P->H1->H2->..._N-1->P

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LEN 64

void showtree();

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send the n_children and word\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[1]), child_id;
    int n_pipes = n_children + 1;
    char buffer[MAX_LEN];

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

        char *msg = argv[2];
        
        printf("\nParent (PID %d) sending the msg: %s\n", getpid() , msg);

        for(int i=0; i < n_pipes; i++){
            if(i != 0){
                close(pipes[i][1]); //no escribe en otros
            }
            
            if(i != n_pipes-1){
                close(pipes[i][0]); //no lee de otros
            }
        }

        write(pipes[0][1], msg, strlen(msg)+1);
        close(pipes[0][1]);

        for(int i=0; i < n_children; i++) wait(NULL);

        read(pipes[n_pipes-1][0], buffer, sizeof(buffer));
        close(pipes[n_pipes-1][0]);

        printf("Parent (PID %d) and the msg is: %s\n", getpid() , buffer);

    }else{ //Child Process
        for(int i=0; i < n_pipes; i++){
            if(i != child_id){
                close(pipes[i][0]);
            }
            
            if(i != child_id + 1){
                close(pipes[i][1]); 
            }
        }

        read(pipes[child_id][0], buffer, sizeof(buffer));
        close(pipes[child_id][0]);

        printf("Child (PID %d) and the msg is: %s\n", getpid() , buffer);

        write(pipes[child_id+1][1], buffer, sizeof(buffer));
        close(pipes[child_id+1][1]);
    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}