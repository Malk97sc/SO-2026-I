#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

void showtree();

int main(int argc, char **argv){
    if(argc < 3){
        perror("Send n childs\n");
        return EXIT_FAILURE;
    }
    pid_t root = getpid();
    int n_children = atoi(argv[1]), child_id;
    int m_grand = atoi(argv[2]), grand_id;
    int level = 0;

    for(child_id=0; child_id < n_children; child_id++){
        if(!fork()){
            level = 1;
            for(grand_id=0; grand_id < m_grand; grand_id++){
                if(!fork()){
                    level = 2;
                    
                    break;
                }
            }
            break;
        }
    }

    if(root == getpid()){ //Parent Process
        showtree();

        for(int i=0; i < n_children * m_grand; i++) wait(NULL);
    }else{ //Child Process
        sleep(1);
        //TU CODIGO

    }

    return EXIT_SUCCESS;
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}