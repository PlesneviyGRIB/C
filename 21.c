#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int cnt;

void handler(int sigNum)
{
    char str[100];

    if (sigNum == SIGQUIT){
        sprintf(str, "The signal has been sent %d times\n", cnt);
        write(STDIN_FILENO, str, strlen(str));
        exit(0);
    }

    cnt++;

    write(STDIN_FILENO, "\a", 1);

    signal(SIGINT, handler);
}

int main()
{
    if(signal(SIGINT, handler) == SIG_ERR) {
        perror("cannot put the handle on SIGINT");
        exit(-1);
    }

    if(signal(SIGQUIT, handler) == SIG_ERR) {
        perror("cannot put the handle on SIGQUIT");
        exit(-1);
    }

    while(1) {
        pause();
    }
}