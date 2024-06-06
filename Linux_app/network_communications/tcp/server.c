#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#define SERVER_PORT 8888
#define BACKLOG 10

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

    signal(SIGCHLD, SIG_IGN);

    SocketServer = socket(AF_INET, SOCK_STREAM, 0);
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

    Ret = listen(SocketServer, BACKLOG);
    if (Ret == -1)
    {
        printf("listen error!\n");
        return -1;
    }

    while(1)
    {
        AddrLen = sizeof(struct sockaddr);
        SocketClient = accept(SocketServer, (struct sockaddr *)&SocketClientAddr, &AddrLen);
        if(SocketClient != -1)
        {
            ClientNum++;
            printf("Get connect from client %d :%s\n", ClientNum, inet_ntoa(SocketClientAddr.sin_addr));
            if(!fork())
            {
                /* 子进程的源码 */
                while(1)
                {
                    /* 接收客户端发来的数据并显示出来 */
                    RecvLen = recv(SocketClient, RecevBuff, 1023, 0);
                    if(RecvLen <= 0)
                    {
                        close(SocketClient);
                        return -1;
                    }
                    else
                    {
                        RecevBuff[RecvLen] = '\0';
                        printf("Get Msg From Client %d :%s\n", ClientNum, RecevBuff);
                    }
                }
            }
        }
    }
    close(SocketServer);
    return 0;
}