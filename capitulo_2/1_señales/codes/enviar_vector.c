#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

int count = 0, flag = 1; //1-ocupado, 0-fin

void showtree();

void signal_handler(int sig);
void send_to(pid_t pid, int signal);
void send_count(pid_t pid, int n_count);

void read_file(const char *file, int **vec, int *size);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file\n");
        return EXIT_FAILURE;
    }
    pid_t child;

    signal(SIGUSR1, signal_handler);
    signal(SIGUSR2, signal_handler);

    if((child = fork())){ //Parent Process
        showtree();
        int *vec, size, value;

        read_file(argv[1], &vec, &size);

        send_count(child, size);

        for(int i=0; i < size; i++){
            value = vec[i];
            send_count(child, value);
        }
        send_to(child, SIGUSR2);

        free(vec);
        wait(NULL);
    }else{ //Child Process
        int size, *result;
        pause();

        while(flag != 0){
            pause();
            count++;
        }
        size = count;
        count = 0;

        printf("Child recived size: %d\n", size);

        result = (int*) malloc(size * sizeof(int));
        if(!result){perror("fail malloc"); exit(1);}
        
        send_to(getppid(), SIGUSR2);
        pause();

        for(int i=0; i < size; i++){
            count = 0;
            while(flag != 0){
                pause();
                count++;
            }
            result[i] = count;
            printf("Child recived size: %d\n", count);
            send_to(getppid(), SIGUSR2);
            pause();
        }

        printf("Child: \n");
        for(int i=0; i < size; i++){
            printf("[%d] ", result[i]);
        }
        printf("\n");

        free(result);
    }

    return EXIT_SUCCESS;
}

void read_file(const char *file, int **vec, int *size){
    FILE *fl = fopen(file, "r");
    if(!fl){
        perror("Fail Fopen\n");
        exit(-1);
    }

    fscanf(fl, "%d", size);
    printf("Size: %d\n", *size);

    *vec = (int*) malloc(*size * sizeof(int));
    if(!*vec){
        perror("Fail malloc\n");
        exit(-1);
    }

    for(int i=0; i < *size; i++){
        if(fscanf(fl, "%d", &(*vec)[i]) != 1){
            fprintf(stderr, "Error: no se pudo leer el elemento %d\n", i);
            exit(EXIT_FAILURE);
        }
    }

    for(int i=0; i < *size; i++){
        printf("%d\n", (*vec)[i]);
    }

    fclose(fl);
}

void send_to(pid_t pid, int signal){
    //printf("[PID %d] sending signal to: %d\n", getpid(), pid);
    usleep(100000);
    kill(pid, signal);
}

void send_count(pid_t pid, int n_count){
    printf("Parent sending: %d\n", n_count);
    for(int i=0; i < n_count; i++){
        send_to(pid, SIGUSR1);
    }
    send_to(pid, SIGUSR2); //no cuentes mas
    pause();
}

void signal_handler(int sig){
    //printf("signal_handler: %d\n", sig);
    if(sig == SIGUSR1){
        flag = 1;
    }else{
        flag = 0;
    }
}

void showtree(){
    char cmd[20] = {""};
    sprintf(cmd, "pstree -cAlp %d", getpid());
    system(cmd);	
}