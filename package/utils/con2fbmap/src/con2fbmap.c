#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/fb.h>
#include <errno.h>


const char *fbpath = "/dev/fb0";                /* any frame buffer will do */

const char *programname;

void Usage(void)
{
    fprintf(stderr, "\nUsage: %s console [framebuffer]\n\n", programname);
    exit(1);
}

int main(int argc, char *argv[])
{
    int do_write = 0;
    int fd;
    struct fb_con2fbmap map;

    programname = argv[0];
    switch (argc) {
        case 3:
            do_write = 1;
            map.framebuffer = atoi(argv[2]);
        case 2:
            map.console = atoi(argv[1]);
            break;
        default:
            Usage();
    }
    if ((fd = open(fbpath, O_RDONLY)) == -1) {
        fprintf(stderr, "open %s: %s\n", fbpath, strerror(errno));
        exit(1);
    }
    if (do_write) {
        if (ioctl(fd, FBIOPUT_CON2FBMAP, &map)) {
            fprintf(stderr, "ioctl FBIOPUT_CON2FBMAP: %s\n", strerror(errno));
            exit(1);
        }
    } else {
        if (ioctl(fd, FBIOGET_CON2FBMAP, &map)) {
            fprintf(stderr, "ioctl FBIOGET_CON2FBMAP: %s\n", strerror(errno));
            exit(1);
        }
        printf("console %d is mapped to framebuffer %d\n", map.console,
               map.framebuffer);
    }
    close(fd);
    exit(0);
}
