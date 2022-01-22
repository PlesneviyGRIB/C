#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>

#define SIZE 1024

char* path = "./tmp";

int main(int argc, char *argv[]) {

    int fd;

    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("function 'socket' error");
        return -1;
    }

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));

    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));

    if (connect(fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("error while connecting");
        return -1;
    }

    char str[SIZE];
    ssize_t cnt;

   // if(setlocale(LC_ALL, "ru_RU.KOI8-R") == NULL)
   //     printf("function 'setlocale' didn't work");

    while((cnt = read(0, str, SIZE)) > 0)
        if(write(fd, str, cnt) != cnt){
            perror("function 'write' error");
        }

    if (cnt == -1){
        perror("function 'read' error");
        return -1;
    }

    close(fd);
    return 0;
}