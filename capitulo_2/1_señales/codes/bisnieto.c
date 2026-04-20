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
    pid_t root = getpid(), *children;
    int n_children = 3, child_id, grand_child, level = 1;

    signal(SIGUSR1, signal_handler);

    children = (pid_t *) malloc(n_children * sizeof(pid_t));
    if(!children){
        perror("Fail malloc");
        return EXIT_FAILURE;
    }

    for(child_id=0; child_id < n_children; child_id++){
        if(!(children[child_id] = fork())){
            if(child_id == 0 || child_id == n_children-1){
                if(!(grand_child = fork())){
                    level = 2;
                    if(!(grand_child = fork())){
                        level = 3;
                    }
                }
            }
            break;
        } 
    }

    if(root == getpid()){ //Parent Process
        showtree();
        printf("Parent\n");
        send_to(children[n_children-1]);
        pause();

        for(int i=0; i < n_children; i++) wait(NULL);
    }else{ //Child Process
        pause();
        //printf("[PID %d]: level: %d\n", getpid(), level);
        switch (level){
            case 1:{
                if(child_id == 0){
                    send_to(grand_child);
                    pause();
                    send_to(getppid());
                }else{
                    if(child_id == n_children-1){
                        send_to(grand_child);
                        pause();
                    }
                    send_to(children[child_id - 1]);
                }
                break;
            }
            case 2:{
                send_to(grand_child);
                pause();
                send_to(getppid());
                break;
            }
            case 3:{
                send_to(getppid());
                break;
            }
        }
    }

    free(children);
    return EXIT_SUCCESS;
}

void send_to(pid_t pid){
    //printf("[PID %d] sending signal to: %d\n", getpid(), pid);
    usleep(10000);
    kill(pid, SIGUSR1);
}

void signal_handler(int sig){

}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}