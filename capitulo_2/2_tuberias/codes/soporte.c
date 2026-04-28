#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>

#define MAX_LEN 125

typedef struct {
    int type; // 0 = urgente, 1 = critico
    char text[MAX_LEN];
} Message;

void showtree();

void read_file(const char *file, char ***vec, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = 2, child_id;
    int n_pipes = 4;
    int pipes[n_pipes][2];

    for(int i=0; i < n_pipes; i++){
        pipe(pipes[i]);
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //Parent Process
        showtree();
        char buff[MAX_LEN], results[MAX_LEN];
        char **vec;
        int size;
        Message msg;
        
        for(int i=0; i < n_pipes; i++){
            if(i != 0) close(pipes[i][1]);
            if(i != 1 && i != 3) close(pipes[i][0]);
        }

        read_file(argv[1], &vec, &size);
        
        for(int i=0; i < size; i++){
            memset(buff, 0, MAX_LEN);
            strncpy(buff, vec[i], MAX_LEN);
            printf("Parent sending: %s\n", buff);
            write(pipes[0][1], buff, MAX_LEN);
        }

        close(pipes[0][1]);

        for(int i=0; i < n_children; i++) wait(NULL);

        printf("\n[LOW PRIORITY]:\n");
        while(read(pipes[1][0], results, MAX_LEN) > 0){
            printf("%s\n", results);
        }
        close(pipes[1][0]);

        while(read(pipes[3][0], &msg, sizeof(Message)) > 0){
            if(msg.type){
                printf("[CRITICAL] \n%s\n\n", msg.text);
            }else{
                printf("[URGENT] \n%s\n\n", msg.text);
            }
        }
        close(pipes[3][0]);
        for(int i=0; i < size; i++) free(vec[i]);
        free(vec);
    }else{ //Child Process
        sleep(1);
        char buff[MAX_LEN];

        if(child_id == 0){
            for(int i=0; i < n_pipes; i++){
                if(i != 0) close(pipes[i][0]);
                if(i != 1 && i != 2) close(pipes[i][1]);
            }

            while(read(pipes[0][0], buff, MAX_LEN) > 0){
                printf("Child %d, recived: %s\n", child_id, buff);

                if((strncmp(buff, "REQ:", 4) == 0 && strchr(buff, ';') != NULL) && strstr(buff, "URGENTE") != NULL){
                    write(pipes[2][1], buff, MAX_LEN);
                }else{
                    write(pipes[1][1], buff, MAX_LEN);
                }
            }         

            close(pipes[0][0]);
            close(pipes[2][1]);
            close(pipes[1][1]);
        }else{
            for(int i=0; i < n_pipes; i++){
                if(i != 2) close(pipes[i][0]);
                if(i != 3) close(pipes[i][1]);
            }
            Message msg;

            while(read(pipes[2][0], buff, MAX_LEN) > 0){
                printf("Child %d, recived: %s\n", child_id, buff);
                
                if(strstr(buff, "servidor") || strstr(buff, "bloqueo") || strstr(buff, "caída")){
                    msg.type = 1; //critico
                }else{
                    msg.type = 0; //urgente
                }
                strncpy(msg.text, buff, MAX_LEN);
                write(pipes[3][1], &msg, sizeof(msg));
            }

            close(pipes[2][0]);
            close(pipes[3][1]);
        }
    }

    return EXIT_SUCCESS;
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

    /*for(int i=0; i < *size; i++){
        printf("%s: and len %ld\n", (*vec)[i], strlen((*vec)[i]));
    }*/

    fclose(fl);
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}