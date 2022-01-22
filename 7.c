#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <signal.h>
#include<sys/mman.h>

struct stat fd;
char* data;
int f;

void func(){
    write(1, "\n", 1);
    write(1, data, fd.st_size);
    write(1, "\n", 1);

    munmap(data, fd.st_size);
    close(f);

    exit(0);
}

int main(int argc, char **argv){
    if (argc < 2) return 0;

    if((f = open(argv[1], O_RDONLY)) == -1){
        perror("function 'open' error");
        return -1;
    }

    if (fstat(f, &fd) == -1) {
        perror("function 'fstat' error");
        return -1;
    }

    if((data = mmap(0, fd.st_size, PROT_READ, MAP_PRIVATE, f, 0)) == MAP_FAILED){
        perror("function 'mmap' error");
        return -1;
    }

    off_t cnt = 0;
    for(off_t i = 0; i<fd.st_size; i++)
        if(data[i] == '\n') cnt++;

    if(data[fd.st_size-1] != '\n') cnt++;

    off_t* sdvig = (off_t*)malloc((cnt+1) * sizeof(off_t));
    off_t pntr = 1;
    off_t cnt_s = 0;

    sdvig[0] = 0;

    for(off_t i = 0; i<fd.st_size; i++)
        if(data[i] != '\n') cnt_s++;
        else {
            sdvig[pntr++] = sdvig[pntr - 1] + cnt_s + 1;
            cnt_s = 0;
        }

    sdvig[cnt] = fd.st_size;

    off_t tmp;
    int ind;
    signal(SIGALRM, func);
    char w = 'r';

    for(;;){
        printf("Enter line number [1..%ld] ",cnt);

        alarm(5);

        while((ind = scanf("%ld", &tmp)) == 0) scanf("%*s\n");

        if(ind == -1) break;

        alarm(0);

        if(tmp != 0){
            if((tmp > 0) && (tmp <= cnt)){
                write(1, data + sdvig[tmp -1], sdvig[tmp] - sdvig[tmp -1]);
            } else printf("Wrong line number!\n");
        } else break;
    }

    munmap(data, fd.st_size);
    close(f);

    return 0;
}