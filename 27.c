#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#define SIZE 1024

int main(int argc, char* argv[])
{
    if(argc != 2){
        fprintf(stderr, "pass one file as a parameter\n");
        return -1;
    }

    int file = open(argv[1], O_RDONLY);
    if(file == -1){
        perror("function 'open' error");
        return -1;
    }

    FILE *pntr;
    if((pntr = popen("wc -c", "w")) == NULL){
        perror("function 'popen' error");
        return -1;
    }

    char str[SIZE];
    int tmp;
    char prev = '\n';

    while((tmp = read(file, str, SIZE)) > 0){
        for(int i = 0; i < tmp; i++){
            if((prev == '\n') && (str[i] == '\n')){
                fputc(' ', pntr);
            }
            prev = str[i];
        }
    }

    if (tmp == -1){
        perror("function 'read' error");
        return -1;
    }

    pclose(pntr);
    close(file);

    return 0;
}