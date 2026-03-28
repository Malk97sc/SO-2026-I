#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void showtree();

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n childs\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[1]), child_id;

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()) break;
    }

    if(root == getpid()){ //Parent Process
        showtree();

        for(int i=0; i < n_children; i++) wait(NULL);
    }else{ //Child Process
        sleep(1);
    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}