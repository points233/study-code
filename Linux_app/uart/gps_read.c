#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

/* set_opt(fd,115200,8,'N',1) */
int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
    struct termios newtio, oldtio;

    if (tcgetattr(fd, &oldtio) != 0)
    {
        perror("SetupSerial 1");
        return -1;
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag |= CLOCAL | CREAD;
    newtio.c_cflag &= ~CSIZE;

    newtio.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); /*Input*/
    newtio.c_oflag &= ~OPOST;                          /*Output*/

    switch (nBits)
    {
    case 7:
        newtio.c_cflag |= CS7;
        break;
    case 8:
        newtio.c_cflag |= CS8;
        break;
    }

    switch (nEvent)
    {
    case 'O':
        newtio.c_cflag |= PARENB;
        newtio.c_cflag |= PARODD;
        newtio.c_iflag |= (INPCK | ISTRIP);
        break;
    case 'E':
        newtio.c_iflag |= (INPCK | ISTRIP);
        newtio.c_cflag |= PARENB;
        newtio.c_cflag &= ~PARODD;
        break;
    case 'N':
        newtio.c_cflag &= ~PARENB;
        break;
    }

    switch (nSpeed)
    {
    case 2400:
        cfsetispeed(&newtio, B2400);
        cfsetospeed(&newtio, B2400);
        break;
    case 4800:
        cfsetispeed(&newtio, B4800);
        cfsetospeed(&newtio, B4800);
        break;
    case 9600:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    case 115200:
        cfsetispeed(&newtio, B115200);
        cfsetospeed(&newtio, B115200);
        break;
    default:
        cfsetispeed(&newtio, B9600);
        cfsetospeed(&newtio, B9600);
        break;
    }

    if (nStop == 1)
        newtio.c_cflag &= ~CSTOPB;
    else if (nStop == 2)
        newtio.c_cflag |= CSTOPB;

    newtio.c_cc[VMIN] = 1;  /* 读数据时的最小字节数: 没读到这些数据我就不返回! */
    newtio.c_cc[VTIME] = 0; /* 等待第1个数据的时间:
                             * 比如VMIN设为10表示至少读到10个数据才返回,
                             * 但是没有数据总不能一直等吧? 可以设置VTIME(单位是10秒)
                             * 假设VTIME=1，表示:
                             *    10秒内一个数据都没有的话就返回
                             *    如果10秒内至少读到了1个字节，那就继续等待，完全读到VMIN个数据再返回
                             */

    tcflush(fd, TCIFLUSH);

    if ((tcsetattr(fd, TCSANOW, &newtio)) != 0)
    {
        perror("com set error");
        return -1;
    }
    // printf("set done!\n");
    return 0;
}

int open_port(char *com)
{
    int fd;
    /* O_NOCTTY意思就是不设置为控制终端
     * 控制终端例如你按下Ctrl+C会使程序中断
     */
    fd = open(com, O_RDWR | O_NOCTTY);
    if (-1 == fd)
    {
        return (-1);
    }
    /* 1.fcntl(fd, F_SETFL, FNDELAY);
     * 读数据时不等待，没有数据就返回0
     * 2.fcntl(fd, F_SETFL, 0);
     * 读数据时，没有数据阻塞
     */
    if (fcntl(fd, F_SETFL, 0) < 0) /* 设置串口为阻塞状态*/
    {
        printf("fcntl failed!\n");
        return -1;
    }

    return fd;
}

int read_gps_raw_data(int fd, char *buf)
{
    int i = 0;
    int iRet;
    char c;
    int start = 0;

    while(1)
    {
        iRet = read(fd, &c, 1);
        if (iRet == 1)
        {
            if (c == '$')
                start = 1;
            if (start)
            {
                buf[i++] = c;
            }
            if (c == '\n' || c == '\r')
                return 0;
        }
        else
        {
            return -1;
        }
    }
}

/* eg. $GPGGA,074529.82,2429.6717,N,11804.6973,E,1,8,1.098,42.110,,,M,,*76<CR><LF> */
int parse_gps_raw_data(char *buf, char *time, char *lat, char *ns, char *lng, char *ew)
{
    char tmp[10];
    if (buf[0] != '$') return -1;
    else if (strncmp(buf + 3, "GGA", 3) != 0) return -1;
    else if(strstr(buf, ",,,,,"))
    {
        printf("raw data: %s\n", buf);
        printf("Place the GPS to open area\n");
        return -1;
    }
    else
    {
        sscanf(buf, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]", tmp, time, lat, ns, lng, ew);
        return 0;
    }
}

/*
 * ./gps_read <dev>
 */
int main(int argc, char **argv)
{
    int fd;
    int iRet;
    char c;
    char buf[1000];
    char time[100];
    char Lat[100];
    char ns[100];
    char Lng[100];
    char ew[100];

    float fLat, fLng;
    if (argc != 2)
    {
        printf("Usage: \n");
        printf("%s </dev/ttySAC1 or other>\n", argv[0]);
        return -1;
    }

    /* 1. open */
    fd = open_port(argv[1]);
    if (fd < 0)
    {
        printf("open %s err!\n", argv[1]);
        return -1;
    }

    /* 2. setup
     * 9600,8N1
     * RAW mode
     * return data immediately
     */
    iRet = set_opt(fd, 9600, 8, 'N', 1);
    if (iRet)
    {
        printf("set port err!\n");
        return -1;
    }

    /* 3. write and read */
    while (1)
    {
        /* eg. $GPGGA,074529.82,2429.6717,N,11804.6973,E,1,8,1.098,42.110,,,M,,*76<CR><LF> */
        /* read line */
        iRet = read_gps_raw_data(fd, buf);
        /* parse line */
        if(iRet == 0)
        {
            iRet = parse_gps_raw_data(buf, time, Lat, ns, Lng, ew);
        }
        /* print */
        if(iRet == 0)
        {
            printf("Time :%s\n", time);
            printf("ns :%s\n", ns);
            printf("ew :%s\n", ew);
            printf("lat :%s\n", Lat);
            printf("lng :%s\n", Lng);

            /* 维度格式：ddmm.mmmm */
            sscanf(Lat + 2, "%f", &fLat);
            fLat = fLat / 60;
            fLat += (Lat[0] - '0') * 10 +  (Lat[0] - '0');

            /* 经度格式 dddmm.mmmm */
            sscanf(Lng + 2, "%f", &fLng);
            fLng = fLng / 60;
            fLng += (Lng[0] - '0') * 100 +  (Lng[0] - '0') * 10 + (Lng[0] - '0');
            printf("Lng,Lat:%.06f,%.06f",fLng, fLat);
        }
    }

    return 0;
}