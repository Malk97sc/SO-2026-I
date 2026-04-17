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
    pid_t root = getpid();

    signal(SIGUSR1, signal_handler);

    if(root == getpid()){ //Parent Process
        showtree();

    }else{ //Child Process
        pause();
    }
    return EXIT_SUCCESS;
}

void send_to(pid_t pid){
    printf("[PID %d] sending signal to: %d\n", getpid(), pid);
    usleep(100000);
    kill(pid, SIGUSR1);
}

void signal_handler(int sig){

}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}