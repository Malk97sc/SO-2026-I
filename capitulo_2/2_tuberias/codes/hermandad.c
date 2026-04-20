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
        perror("Send the n_children\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[1]), child_id;
    char buffer[MAX_LEN], buffer_out[MAX_LEN];
    
    int in_pipes[n_children][2];
    int out_pipes[n_children][2];
    

    for(int i=0; i < n_children; i++){
        if(pipe(in_pipes[i]) == -1){perror("Fail pipe"); exit(1);}
        if(pipe(out_pipes[i]) == -1){perror("Fail pipe"); exit(1);}
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //Parent Process
        showtree(); 

        for(int i=0; i < n_children; i++){
            if(i == 0){
                close(in_pipes[i][0]);
                close(out_pipes[i][1]);
            }else{
                close(in_pipes[i][0]); close(in_pipes[i][1]); 
                close(out_pipes[i][0]); close(out_pipes[i][1]);
            }
        }

        printf("Send msg: \n");
        if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            perror("fail fgets");
            exit(1);
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        printf("Parent sending: %s\n", buffer);
        write(in_pipes[0][1], buffer, sizeof(buffer));
        close(in_pipes[0][1]);

        read(out_pipes[0][0], buffer_out, sizeof(buffer_out));
        close(out_pipes[0][0]);

        printf("Parent reciving: %s\n", buffer_out);

        for(int i=0; i < n_children; i++) wait(NULL);
    }else{ //Child Process
        
        read(in_pipes[child_id][0], buffer, sizeof(buffer));
        close(in_pipes[child_id][0]);
        printf("[Child %d] recived from %d in: %s\n", child_id, child_id, buffer);

        if(child_id == n_children-1){
            printf("[Child %d] Sending to out: %s\n", child_id, buffer);
            write(out_pipes[child_id][1], buffer, sizeof(buffer));
            close(out_pipes[child_id][1]);
        }else{
            printf("[Child %d] Sending to %d in: %s\n", child_id, child_id+1, buffer);
            write(in_pipes[child_id+1][1], buffer, sizeof(buffer));
            close(in_pipes[child_id+1][1]);

            read(out_pipes[child_id+1][0], buffer_out, sizeof(buffer_out));
            close(out_pipes[child_id+1][0]);
            printf("[Child %d] recived from %d out: %s\n", child_id, child_id+1, buffer_out);

            write(out_pipes[child_id][1], buffer_out, sizeof(buffer_out));
            close(out_pipes[child_id][1]);
            printf("[Child %d] Sending to %d out: %s\n", child_id, child_id, buffer_out);
        }

    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}