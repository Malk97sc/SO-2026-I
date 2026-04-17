#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

void showtree();

void signal_handler(int sig);
void send_to(pid_t pid);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n childs\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid(), *children;
    int n_children = atoi(argv[1]), child_id;

    signal(SIGUSR1, signal_handler);

    children = (pid_t *) malloc(n_children * sizeof(pid_t));
    if(!children){
        perror("Fail malloc");
        return EXIT_FAILURE;
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!(children[child_id] = fork())) break;
    }

    if(root == getpid()){ //Parent Process
        showtree();
        printf("Parent\n");
        send_to(children[n_children-1]);
        pause();

        printf("Parent\n");

        for(int i=0; i < n_children; i++) wait(NULL);
    }else{ //Child Process
        pause();
        printf("H%d\n", child_id);
        if(child_id == 0){
            send_to(getppid());
        }else{
            send_to(children[child_id - 1]);
        }
    }

    free(children);
    return EXIT_SUCCESS;
}

void send_to(pid_t pid){
    printf("[PID %d] sending signal to: %d\n", getpid(), pid);
    usleep(1000000);
    kill(pid, SIGUSR1);
}

void signal_handler(int sig){

}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}