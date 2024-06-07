#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define SERVER_PORT 8888

int main(int argc, char ** argv)
{
    int SocketServer;
    int SocketClient;
    int Ret;
    int AddrLen;
    int RecvLen;
    int ClientNum = -1;
    unsigned char RecevBuff[1024];
    struct sockaddr_in SocketServerAddr;
    struct sockaddr_in SocketClientAddr;


    SocketServer = socket(AF_INET, SOCK_DGRAM, 0);
    if (SocketServer == -1)
    {
        printf("socket error!\n");
        return -1;
    }

    SocketServerAddr.sin_family         = AF_INET;
    SocketServerAddr.sin_port           = htons(SERVER_PORT); /* host to net , short*/
    SocketServerAddr.sin_addr.s_addr    = INADDR_ANY;   /* 本机上所有ip */
    memset(SocketServerAddr.sin_zero, 0, 8);

    Ret = bind(SocketServer, (const struct sockaddr *)&SocketServerAddr,sizeof(struct sockaddr));
    if (Ret == -1)
    {
        printf("bind error!\n");
        return -1;
    }

    while(1)
    {
        AddrLen = sizeof(struct sockaddr);
        RecvLen = recvfrom(SocketServer, RecevBuff, 1023, 0, (struct sockaddr *)&SocketClientAddr, &AddrLen);
        if(RecvLen > 0)
        {
            RecevBuff[RecvLen] = '\0';
            printf("Get Msg From %s :%s\n", inet_ntoa(SocketClientAddr.sin_addr), RecevBuff);
        }
    }
    close(SocketServer);
    return 0;
}