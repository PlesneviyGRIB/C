#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>

char *path = "./socket";

int client_reader(int descriptor, int *array) {
    char buff[512];
    ssize_t reader;
    reader = read(descriptor, buff, sizeof(buff));
    if (reader < 0) {
        perror("error while reading");
        return -1;
    } else {
        if (reader == 0) {
            return -1;
        } else {
            int i = 0;
            while (array[i] != -1 ) {
              if (array[i]  == descriptor) {
                i++;
                continue;
              }
              write(array[i], buff, reader);
              i++;
            }
        }
    }
    return 0;
}

void sig_handler(int signo) {
    unlink(path);
    exit(0);
}

void unlinker() {
    unlink(path);
}


int array_descriptors[FD_SETSIZE];
int count_busy = 0;

void add_descriptor(int desc) {
    array_descriptors[count_busy] = desc;
    count_busy++;
    array_descriptors[count_busy] = -1;
}

void delete_descriptor(int desc) {
    for (int i=0; i<FD_SETSIZE; i++){
        if (array_descriptors[i] == desc){
            for(int j = i; j < count_busy; j++){
                array_descriptors[j] = array_descriptors[j+1];
            }
            count_busy--;
            array_descriptors[count_busy] = -1;
            return;
        }
    }
    perror("something strange with descriptor\n");
}

int main() {
    signal(SIGINT, sig_handler);
    int descriptor;
    struct sockaddr_un adrress;
    fd_set active_set, read_set;
    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Can't create socket");
        return -1;
    }
    for (int i = 0; i < FD_SETSIZE; i++) {
        array_descriptors[i] = -1;
    }
    array_descriptors[count_busy] = descriptor;
    array_descriptors[count_busy + 1] = -1;
    adrress.sun_family = AF_UNIX;
    memset(adrress.sun_path, 0, sizeof(adrress.sun_path));
    strncpy(adrress.sun_path, path, sizeof(adrress.sun_path) - 1);
    unlink(path);
    if (bind(descriptor, (struct sockaddr *) &adrress, sizeof(adrress)) == -1) {
        perror("error while binding");
        return -1;
    }
    if (atexit(unlinker) != 0) {
        perror("error while atexit");
        return -1;
    }
    if (listen(descriptor, 5) == -1) {
        perror("error while listening");
        return -1;
    }
    FD_ZERO(&active_set);
    FD_SET(descriptor, &active_set);
    add_descriptor(descriptor);
    while (1) {
        read_set = active_set;
        if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0) {
            perror("error while selecting");
            return -1;
        }
        int i = 0;
        while(array_descriptors[i] != -1){
            if( FD_ISSET(array_descriptors[i], &read_set)){
                if(array_descriptors[i] == descriptor){
                    int current_client = accept(descriptor, NULL, NULL);
                    if (current_client<0){
                        perror("error while accepting");
                    }
                    add_descriptor(current_client);
                    FD_SET(current_client, &active_set);
                } else {
                    if (client_reader(array_descriptors[i], array_descriptors+1)<0){
                        close(array_descriptors[i]);
                        FD_CLR(array_descriptors[i], &active_set);
                        delete_descriptor(array_descriptors[i]);
                    }

                }
            }
            i++;
        }
        }
}















































/*
#include <ctype.h>
#include <signal.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>

char *path = "./socket";

int read_from_client(int filedes) {
    char buffer[512];
    ssize_t nbytes;
    nbytes = read(filedes, buffer, sizeof(buffer));
    if (nbytes < 0) {
        perror("read");
        exit(EXIT_FAILURE);
    } else {
        if (nbytes == 0) {
            return -1;
        } else {
            for (int i = 0; i < nbytes; i++) {
                putchar(toupper(buffer[i]));
            }
        }
    }
    return 0;
}

void sig_handler(int signo) {
    unlink(path);
    exit(0);
}

void unlinker() {
    unlink(path);
}


int array_descriptors[FD_SETSIZE];
int count_busy = 0;

void add_descriptor(int desc) {
    array_descriptors[count_busy] = desc;
    count_busy++;
    array_descriptors[count_busy] = -1;
}

void delete_descriptor(int desc) {
    for (int i=0; i<FD_SETSIZE; i++){
        if (array_descriptors[i] == desc){
            for(int j = i; j < count_busy; j++){
                array_descriptors[j] = array_descriptors[j+1];
            }
            count_busy--;
            array_descriptors[count_busy] = -1;
            return;
        }
    }
    perror("something strange with descriptor\n");
}

int main() {
    signal(SIGINT, sig_handler);
    int my_socket;
    struct sockaddr_un addr;
    fd_set active_fd_set, read_fd_set;
    if ((my_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < FD_SETSIZE; i++) {
        array_descriptors[i] = -1;
    }
    array_descriptors[count_busy] = my_socket;
    array_descriptors[count_busy + 1] = -1;
    addr.sun_family = AF_UNIX;
    memset(addr.sun_path, 0, sizeof(addr.sun_path));
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    unlink(path);
    if (bind(my_socket, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        perror("Can't bind socket");
        exit(EXIT_FAILURE);
    }
    if (atexit(unlinker) != 0) {
        perror("error while atexit");
        return -1;
    }
    if (listen(my_socket, 5) == -1) {
        perror("Can't listen for connections");
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&active_fd_set);
    FD_SET(my_socket, &active_fd_set);
    add_descriptor(my_socket);
    while (1) {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror("Can't select");
            exit(EXIT_FAILURE);
        }

        int i = 0;
        while(array_descriptors[i] != -1){
            if( FD_ISSET(array_descriptors[i], &read_fd_set)){
                if(array_descriptors[i] == my_socket){
                    int new = accept(my_socket, NULL, NULL);
                    if (new<0){
                        perror("error while accepting");
                    }
                    add_descriptor(new);
                    FD_SET(new, &active_fd_set);
                } else {
                    if (read_from_client(array_descriptors[i])<0){
                        close(array_descriptors[i]);
                        FD_CLR(array_descriptors[i], &active_fd_set);
                        delete_descriptor(array_descriptors[i]);
                    }

                }
            }
            i++;
        }
        }
}

*/































/*
#include <ctype.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/un.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
char *path = "./socket";
int read_from_client(int filedes)
{
    char buffer[512];
    ssize_t nbytes;
    nbytes = read(filedes, buffer, sizeof(buffer));
    if (nbytes < 0)
    {
        perror("read");
        exit(EXIT_FAILURE);
    }
    else
    {
        if (nbytes == 0)
        {
            return -1;
        }
        else
        {
            for (int i = 0; i < nbytes; i++)
            {
                putchar(toupper(buffer[i]));
            }
        }
    }
    return 0;
}

void sig_handler(int signo)
{
    unlink(path);
    exit(0);
}
void unlinker(){
    unlink(path);
}

int main()
{
    signal(SIGINT, sig_handler);
    int my_socket;
    struct sockaddr_un addr;
    fd_set active_fd_set, read_fd_set;
    if ((my_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Can't create socket");
        exit(EXIT_FAILURE);
    }
    addr.sun_family = AF_UNIX;
    memset(addr.sun_path, 0, sizeof(addr.sun_path));
    strncpy(addr.sun_path, path, sizeof(addr.sun_path) - 1);
    unlink(path);
    if (bind(my_socket, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("Can't bind socket");
        exit(EXIT_FAILURE);
    }
    if (atexit(unlinker)!=0){
        perror("error while atexit");
        return -1;
    }
    if (listen(my_socket, 5) == -1)
    {
        perror("Can't listen for connections");
        exit(EXIT_FAILURE);
    }
    FD_ZERO(&active_fd_set);
    FD_SET(my_socket, &active_fd_set);
    int i;
    while (1)
    {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
        {
            perror("Can't select");
            exit(EXIT_FAILURE);
        }
        for (i = 0; i < FD_SETSIZE; ++i)
            if (FD_ISSET(i, &read_fd_set))
            {
                if (i == my_socket)
                {
                    int new;
                    new = accept(my_socket,NULL,NULL);
                    if (new < 0)
                    {
                        perror("Can't accept");
                        exit(EXIT_FAILURE);
                    }
                    FD_SET(new, &active_fd_set);
                }
                else
                {
                    if (read_from_client(i) < 0)
                    {
                        close(i);
                        FD_CLR(i, &active_fd_set);
                    }
                }
            }
    }
    return 0;
}
*/
