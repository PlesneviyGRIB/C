#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <fcntl.h>

main() {
        struct termios tty, ttyCopy;
        int fd;
        fd = open("/dev/tty", O_RDONLY);

        tcgetattr(fd, &tty);
        ttyCopy = tty;

        tty.c_lflag &= ~(ISIG | ICANON);
        tty.c_cc[VMIN] = 1;

        tcsetattr(fd, TCSAFLUSH, &tty);

        printf("Yes or no?\n");

        char c;
        read(fd, &c, 1);

        printf("\nOk.\n");
        tcsetattr(fd, TCSAFLUSH, &ttyCopy);

        exit(0);
}