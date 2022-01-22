#include <unistd.h>
#include <stdio.h>
#include <sys/un.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <locale.h>

#define SIZE 1024

char* path = "./tmp";

void func(){
    unlink(path);
    exit(0);
}

int main(int argc, char *argv[]){

    int fd;

    if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("function 'socket' error");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));

    signal(SIGINT, func);

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1){
        perror("function 'bind' error");
        func();
    }

    if (listen(fd, 1) == -1){
        perror("function 'listen' error");
        func();
    }

    int fd_client;

    if ((fd_client = accept(fd, NULL, NULL)) == -1) {
        perror("function 'accept' error");
        func();
    }

    unlink(path);

    char str[SIZE];
    ssize_t cnt;

    char* p;
    if((p = setlocale(LC_ALL, "ru_RU.KOI8-R")) == NULL)
        printf("function 'setlocale' didn't work\n");

    while ((cnt = read(fd_client, str, SIZE)) > 0)
        for(int i = 0; i < cnt; i++)
            putchar(toupper(str[i]));

    if (cnt == -1){
        perror("function 'read' error");
        return -1;
    }

    close(fd_client);
    close(fd);
    
    return 0;
}