#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#define SIZE 1024

int main(int argc, char **argv){
    if (argc < 2) return 0;
    int f;

    if((f = open(argv[1], O_RDONLY)) == -1){
        perror("function 'open' error");
        return -1;
    }

    off_t cur = 10;
    off_t cnt = 0;
    char s[SIZE];
    ssize_t num = 0;
    off_t* sdvig = (off_t*)malloc(cur * sizeof(off_t));
    off_t pntr = 1;
    off_t tmp = 0;
    sdvig[0] = 0;
    char w;

    while((num = read(f, &s, SIZE)) >0 ){
        for(ssize_t i = 0; i<num; i++){
            w = s[i];
            if(s[i] == '\n'){
                cnt++;

                if(pntr >= cur){
                    cur *= 2;
                    sdvig = (off_t*)realloc(sdvig, cur*sizeof(off_t));
                }

                sdvig[pntr] = lseek(f, 0, SEEK_CUR) - num + i +1;
                if((sdvig[pntr] - sdvig[pntr - 1]) > tmp) tmp = sdvig[pntr] - sdvig[pntr - 1];
                pntr++;
            }
        }
    };

    if(w != '\n') cnt++;

    sdvig[cnt] = lseek(f, 0, SEEK_CUR);
    char str[tmp + 3];
    int ind;
    w = 'r';

    for(;;){
        printf("Enter line number [1..%ld] ",cnt);

        while((ind = scanf("%ld", &tmp)) == 0)
        {
            scanf("%*s\n");
        }

        if(ind == -1) break;

        if(tmp != 0){
            if((tmp > 0) && (tmp <= cnt)){
                lseek(f, sdvig[tmp - 1], SEEK_SET);
                read(f, str, sdvig[tmp] - sdvig[tmp - 1]);
                write(1, str, sdvig[tmp] - sdvig[tmp - 1]);
            } else printf("wrong line number!\n");
        } else break;
    }

    free(sdvig);
    close(f);

    return 0;
}