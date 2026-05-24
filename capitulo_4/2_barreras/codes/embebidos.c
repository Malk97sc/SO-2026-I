#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINES 3
#define MAX_LEN 256

typedef enum {
    WORKER_LINES,
    WORKER_KEYWORDS,
    WORKER_COMMENTS
} worker_type;

typedef struct {
    int id;
    worker_type type;
} thread_args;

char buffer[MAX_LINES][MAX_LEN];

int n_threads;

int read_count = 0;
int done = 0;

//shared data

int lines;
int keywords;
int comments;

pthread_mutex_t mutex;
pthread_barrier_t barrier_start;
pthread_barrier_t barrier_end;

const char *reserved_words[] = {
    "int", "char", "float",
    "if", "else", "while"
};

int size = sizeof(reserved_words) / sizeof(reserved_words[0]);

void create_threads(const char *file, int n_threads);

void read_file(const char *file);

void *thread_lines(void *arg);
void *thread_keywords(void *arg);
void *thread_comments(void *arg);

void *work(void *arg);

int count_comments(const char *line);
int count_lines(const char *line);
int count_keywords(const char *line);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send n_threads");
        return EXIT_FAILURE;
    }
    n_threads = 3;

    create_threads(argv[1], n_threads);

    printf("\nFinal Results\n");
    printf("Lines: %d\n", lines);
    printf("Keywords: %d\n", keywords);
    printf("Comments: %d\n", comments);

    return EXIT_SUCCESS;
}

void create_threads(const char *file, int n_threads){
    pthread_t *threads; //n_threads
    thread_args *args;

    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier_start, NULL, n_threads+1);
    pthread_barrier_init(&barrier_end, NULL, n_threads+1);

    threads = (pthread_t*) malloc(n_threads * sizeof(pthread_t));
    args = (thread_args*) malloc(n_threads * sizeof(thread_args));
    if(!threads || !args){
        perror("Fail malloc");
        exit(-1);
    }

    args[0].id = 1; 
    args[0].type = WORKER_LINES; 
    
    args[1].id = 2;
    args[1].type = WORKER_KEYWORDS;

    args[2].id = 3;
    args[2].type = WORKER_COMMENTS;

    /*pthread_create(&threads[0], NULL, thread_lines, &args[0]);
    pthread_create(&threads[1], NULL, thread_keywords, &args[1]);
    pthread_create(&threads[2], NULL, thread_comments, &args[2]);*/

    for(int i=0; i < n_threads; i++){
        pthread_create(&threads[i], NULL, work, &args[i]);
    }

    read_file(file);

    for(int i=0; i < n_threads; i++){
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barrier_start);
    pthread_barrier_destroy(&barrier_end);
    free(threads);
    free(args);
}

void *work(void *arg){
    thread_args *t = (thread_args*) arg;
    int local_count;

    while(1){
        pthread_barrier_wait(&barrier_start);
        if(read_count == 0 && done){
            pthread_barrier_wait(&barrier_end);
            break;
        }

        local_count = 0;

        for(int i=0; i < read_count; i++){
            switch(t->type){
                case WORKER_LINES:{
                    local_count += count_lines(buffer[i]);
                    break;
                }
                case WORKER_KEYWORDS:{
                    local_count += count_keywords(buffer[i]);
                    break;
                }
                case WORKER_COMMENTS:{
                    local_count += count_comments(buffer[i]);
                    break;
                }
            }

            printf("[THREAD %d], processing: %s\n", t->id, buffer[i]);
        }

        pthread_mutex_lock(&mutex);
        switch(t->type){
            case WORKER_LINES:{
                lines += local_count;
                break;
            }
            case WORKER_KEYWORDS:{
                keywords += local_count;
                break;
            }
            case WORKER_COMMENTS:{
                comments += local_count;
                break;
            }
        }
        pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier_end);
    }

    return NULL;
}

void *thread_lines(void *arg){
    thread_args *t = (thread_args*) arg;
    int local_count;

    while(1){
        pthread_barrier_wait(&barrier_start);
        if(read_count == 0 && done){
            pthread_barrier_wait(&barrier_end);
            break;
        }

        local_count = 0;

        for(int i=0; i < read_count; i++){
            local_count += count_lines(buffer[i]);

            printf("[THREAD %d], processing: %s\n", t->id, buffer[i]);
        }

        //pthread_mutex_lock(&mutex);
        lines += local_count;
        //pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier_end);
    }

    return NULL;
}
int count_lines(const char *line){
    while(isspace(*line)){
        line++; // '       '
    }

    return (*line != '\0');
}

void *thread_keywords(void *arg){
    thread_args *t = (thread_args*) arg;
    int local_count;

    while(1){
        pthread_barrier_wait(&barrier_start);
        if(read_count == 0 && done){
            pthread_barrier_wait(&barrier_end);
            break;
        }
        local_count = 0;

        for(int i=0; i < read_count; i++){
            local_count += count_keywords(buffer[i]);

            printf("[THREAD %d], processing: %s\n", t->id, buffer[i]);
        }

        //pthread_mutex_lock(&mutex);
        keywords += local_count;
        //pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier_end);
    }

    return NULL;
}
int count_keywords(const char *line){
    int total = 0;
    char copy[MAX_LEN];
    char *token;

    strncpy(copy, line, MAX_LEN);

    token = strtok(copy, " \t(){};,");

    while(token){

        for(int i=0; i < size; i++){
            if(strcmp(token, reserved_words[i]) == 0){
                total++;
            }
        }

        token = strtok(NULL, " \t(){};,");
    }

    return total;
}

void *thread_comments(void *arg){
    thread_args *t = (thread_args*) arg;
    int local_count;

    while(1){
        pthread_barrier_wait(&barrier_start);
        if(read_count == 0 && done){
            pthread_barrier_wait(&barrier_end);
            break;
        }

        local_count = 0;

        for(int i=0; i < read_count; i++){
            local_count += count_comments(buffer[i]);

            printf("[THREAD %d], processing: %s\n", t->id, buffer[i]);
        }

        //pthread_mutex_lock(&mutex);
        comments += local_count;
        //pthread_mutex_unlock(&mutex);

        pthread_barrier_wait(&barrier_end);
    }

    return NULL;
}
int count_comments(const char *line){
    if(strstr(line, "//") || strstr(line, "/*")){
        return 1;
    }
    return 0;
}

void *func(void *arg){
    thread_args *thread = (thread_args*) arg;

    printf("Thread: %d start\n", thread->id);

    //pthread_barrier_wait(&barrier);

    printf("Thread: %d ending\n", thread->id);

    return NULL;
}

void read_file(const char *file){
    FILE *fl = fopen(file, "r");
    if(!fl) exit(-1);
    
    int round = 0;
    size_t len;

    while(1){
        read_count = 0;

        for(int i=0; i < MAX_LINES; i++){
            if(!fgets(buffer[i], MAX_LEN, fl)){
                break;
            }

            len = strlen(buffer[i]);
            if(len > 0 && buffer[i][len - 1] == '\n'){
                buffer[i][len - 1] = '\0';
            }

            read_count++;
        }

        round++;

        if(read_count == 0){
            done = 1;
        }

        printf("\n[MAIN] Round: %d\n", round);

        pthread_barrier_wait(&barrier_start);

        pthread_barrier_wait(&barrier_end);

        if(done){
            break;
        }
    }

    fclose(fl);
}