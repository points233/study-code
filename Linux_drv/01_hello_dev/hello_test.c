#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

/* 写: ./hello_test /dev/xxx point 
 * 读: ./hello_test /dev/xxx
 */
int main(int argc, char **argv)
{
    int fd;
    int len;
    char buf[100];
    if(argc < 2)
    {
        printf("Usage: \n");
        printf("%s <dev> [string]\n", argv[0]);
        return -1;
    }
    // open
    fd = open(argv[1], O_RDWR);
    if(fd < 0)
    {
        perror("can not open file %s\n", argv[1]);
        return -1;
    }
    // write
    if(argc == 3)
    {
        len = write(fd, argv[2], strlen(argv[2]) + 1);
        printf("write ret = %d\n", len);
    }
    else
    {
        // read
        len = read(fd, buf, 100);
        buf[99] = '\0';
        printf("read str = %s\n", buf);
    }
    
    // close

    return 0;
}