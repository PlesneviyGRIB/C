#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <strings.h>
#include <string.h>
#include <stdlib.h>

int searchStr(char* str, char* mpart, int pos) //search for a substring in a string
{
    if (pos + strlen(mpart) > strlen(str)) return -1;

    for(int i = pos; i < strlen(str); i++) {
        if((str[i] == mpart[0]) || (mpart[0] == '?')) {
            int j = i;
            int k = 0;
            while(((str[j] == mpart[k]) || (mpart[k] == '?')) && (k < strlen(mpart)) && (j < strlen(str))) {
                j++;
                k++;
            }
            if(k == strlen(mpart)) return i + k;
        }
    }
    return -1;
}

int searchStr1(char* str, char* mpart, int pos) //search for a substring in a string
{
    if (pos + strlen(mpart) > strlen(str)) return -2;

    for(int i = pos; i < strlen(str); i++) {
        if((str[i] == mpart[0]) || (mpart[0] == '?')) {
            int j = i;
            int k = 0;
            while(((str[j] == mpart[k]) || (mpart[k] == '?')) && (k < strlen(mpart)) && (j < strlen(str))) {
                j++;
                k++;
            }
            if(k == strlen(mpart)) return i;
        }
    }
    return -2;
}

char* normalMask(char* mask) //transform mask **asd**??***  ->  *asd*??*
{
    char* m = (char*)malloc(256 * sizeof(char));
    if(m == NULL) {
        perror("memory allocation error");
        exit(-1);
    }
    int pos = 0;

    int ind = 0;
    for(int i = 0; i<strlen(mask); i++) {
        if((i > 0) && (mask[i-1] == '*')) ind = 1;

        if(!ind) {
            m[pos++] = mask[i]; 
        }
        else {
            if(mask[i] != '*') {
                m[pos++] = mask[i]; 
            }
        }
        ind = 0;
    }
    m[pos] = '\0';

    return m;
}

void freee(char** arr, int cnt)
{
    for(int i = 0; i < cnt; i++) free(arr[i]);
    free(arr);
}

int check(char* str, char* mask) //function to check if str corresponds to mask
{
    if(str[0] == '.') return 0;

    mask = normalMask(mask);
    int mcnt = 1;
    int mln = strlen(mask);

    for (int i = 0; i < mln; i++)
        if (mask[i] == '*') mcnt++;

    if(mask[0] == '*') mcnt--;
    if(mask[mln-1] == '*') mcnt--;


    //array of substrings
    char** mstrs = (char**)malloc(mcnt * sizeof(char*));
    if(mstrs == NULL) {
        perror("memory allocation error");
        exit(-1);
    }

    for(int i = 0; i < mcnt; i++)
    {
        mstrs[i] = (char*)malloc(256 * sizeof(char));
        if(mstrs[i] == NULL) {
            perror("memory allocation error");
            exit(-1);
        }
        mstrs[i][0] = '\0';
    }

    int j = 0;
    int k = 0;

    for (int i = 0; i < mln; i++)
    {
        if ((mask[i] == '*') && (i != 0) && (i != mln-1))
        {
            j++;
            k = 0;
        }
        else
        {
            if(mask[i] != '*') {
                mstrs[j][k] = mask[i];
                mstrs[j][++k] = '\0';
            }
        }
    }

    int pos = 0;

    //checking each substring
    for(int i = 0;i < mcnt; i++) {
        pos = searchStr(str, mstrs[i], pos);
        if(pos == -1) {
            free(mask);
            freee(mstrs, mcnt);
            return 0;
        }
    }

    if ((pos != strlen(str)) && (mask[strlen(mask)-1]) != '*') {
        int ind = 0;
        for(int i=0; i<strlen(mstrs[mcnt-1]); i++) {
            if(mstrs[mcnt-1][i] != '?') {
                ind = 1;
                break;
            }
        }

        if(ind != 0) {
            int k = 0;

            while(k>-1) {
                k = searchStr1(str,mstrs[mcnt - 1],k);
                if(k + strlen(mstrs[mcnt - 1]) == strlen(str)) {
                    free(mask);
                    freee(mstrs, mcnt);
                    return 1;
                }
                k++;
            }
        }

        if(ind == 1) {
            free(mask);
            freee(mstrs, mcnt);
            return 0;
        }

        if((mcnt == 1) && ((mask[0]) != '*') && (ind == 0)) {
            free(mask);
            freee(mstrs, mcnt);
            return 0;
        }

        int p = -1;
        for(int i = 0; i<strlen(mask);i++) {
            if(mask[i] == '*') p = i;
        }

        if((ind == 0) && (p != -1)) {
            free(mask);
            freee(mstrs, mcnt);
            return 1;
        }
    }
    
    if(mask[0] != '*') {
        if(searchStr(str,mstrs[0], 0) != strlen(mstrs[0])) {
            free(mask);
            freee(mstrs, mcnt);
            return 0;
        }
    }

    free(mask);
    freee(mstrs, mcnt);

    return 1;
}

void lookup(char* arg)
{
    int i = 0;
    while(arg[i] != '\0'){
        if(arg[i] == '/') {
            fprintf(stderr,"mask cannot contain '/'\n");
            return;
        }
        i++;
    }

    DIR* dirp;
    struct dirent* dp;
    int ind = 0;

    if ((dirp = opendir(".")) == NULL) {
        perror("couldn't open '.'");
        return;
    }

    do {
        errno = 0;
        if ((dp = readdir(dirp)) != NULL) {
            if (check(dp->d_name, arg) == 1) {
                printf("%s\n", dp->d_name);
                ind = 1;
            }
        }
    } while (dp != NULL);


    if (errno != 0) {
        perror("error reading directory");
    }
    else {
        if (ind == 0) printf("%s", arg);
    }

    printf("\n");
    closedir(dirp);
    return;
}

int main(int argc, char* argv[])
{
    char str[256];

    printf("Print file mask: ");
    scanf("%256s", str);

    lookup(str);

    return (0);
}