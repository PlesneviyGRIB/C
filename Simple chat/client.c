//
//  clientSlavin.c
//  socket_parallel
//
//  Created by Кирилл Бронников on 22.12.2021.
//

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char *path = "./socket";

int main() {
    struct sockaddr_un address;
    char buff[BUFSIZ];
    int descriptor;
    if ((descriptor = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
    {
        perror("Can't create socket");
        return -1;
    }
    address.sun_family = AF_UNIX;
    memset(address.sun_path, 0, sizeof(address.sun_path));
    strncpy(address.sun_path, path, sizeof(address.sun_path) - 1);
    if (connect(descriptor, (struct sockaddr *)&address, sizeof(address)) == -1)
    {
        perror("error while connecting");
        return -1;
    }
    memset(buff, 0, BUFSIZ);




    int descr[] = {STDIN_FILENO, descriptor};
    fd_set active_fd_set;
    FD_ZERO(&active_fd_set);
    FD_SET(descriptor, &active_fd_set);
    FD_SET(STDIN_FILENO, &active_fd_set);

fd_set read_fd_set;
    while (1) {
        read_fd_set = active_fd_set;
        if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
            perror("Can't select");
            exit(EXIT_FAILURE);
        }
        int fl = 0;
        for (int i = 0; i < 2; i++) {
            if (FD_ISSET(descr[i], &read_fd_set)) {
                char buff[100];
                if (descr[i] == STDIN_FILENO) {
                    ssize_t g;
                    if ((g = read(STDIN_FILENO, buff, 100)) > 0) {
                        write(descriptor, buff, g); // TODO perror
                    } else{
                        // stdin closed -> time to exit from application
                        close(descriptor);

                        return 0;
                      }
                    } else if(descr[i] == descriptor){
                      ssize_t g;
                      if ((g = read(descriptor, buff, 100)) > 0) {
                        write(STDOUT_FILENO, buff, g); // TODO perror
                      } else{
                        close(descriptor);
                        return 0;
                      }
                    }
                  }
                }
              }
}
