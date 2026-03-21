#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* read_file(const char *file);
char *encoder(const char *str);

int main(int argc, char **argv){
    if(argc < 2){
        perror("Send file\n");
        return EXIT_FAILURE;
    }
    char *buffer, *result;

    buffer = read_file(argv[1]);
    
    result = encoder(buffer);
    
    printf("%s\n", result);

    free(buffer);
    free(result);
    return EXIT_SUCCESS;
}

char *encoder(const char *str){
    int size = strlen(str), count = 0, j;
    char tok, *result_buff, buffer[20];

    result_buff = (char *) malloc(size * 2 * sizeof(char));
    result_buff[0] = '\0';

    for(int i=0; i < size; i++){
        tok = str[i];
        count = 0;
        j = i;

        while(j < size && str[j] == tok){
            count++;
            j++;
        }
        i = j-1;
        snprintf(buffer, sizeof(buffer), "%c%d", tok, count);
        strcat(result_buff, buffer);
        //printf("%c%d", tok, count);
    }

    return result_buff;
}

char* read_file(const char *file){
    FILE *fl = fopen(file, "r");
    if(!fl){perror("Fail fopen\n"); return NULL;}
    char *buffer = NULL;
    size_t size = 0, amount_read;

    amount_read = getline(&buffer, &size, fl);
    if(amount_read == -1){
        perror("Fail getline\n");
        return NULL;
    }

    printf("Word: %s\n", buffer);
    buffer[amount_read-1] = '\0';
    /*printf("READ: %ld, Last character: %c\n", amount_read, buffer[amount_read-1]);
    
    for(int i=0; i < amount_read; i++){
        printf("[%d] = %c (%d)\n", i, buffer[i], buffer[i]);
    }*/

    fclose(fl);
    return buffer;
}