#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>

#define SERVER_PORT 8888

int main(int argc, char ** argv)
{
    int SocketClient;
    int Ret;
    int SendLen;
    int AddrLen;
    unsigned char SendBuff[1024];
    struct sockaddr_in SocketServerAddr;
    
    if (argc != 2)
    {
        printf("Usage\n");
        printf("%s <server_ip>", argv[0]);
        return -1;
    }
    
    SocketClient = socket(AF_INET, SOCK_DGRAM, 0);
    
    SocketServerAddr.sin_family         = AF_INET;
    SocketServerAddr.sin_port           = htons(SERVER_PORT); /* host to net , short*/
    // SocketServerAddr.sin_addr.s_addr    = INADDR_ANY;   /* 本机上所有ip */
    if (inet_aton(argv[1], &SocketServerAddr.sin_addr) == 0)
    {
        printf("invalued server_ip\n");
        return -1;
    }
    memset(SocketServerAddr.sin_zero, 0, 8);
#if 0
    Ret = connect(SocketClient, (const struct sockaddr *)&SocketServerAddr, sizeof(struct sockaddr));
    if (Ret == -1)
    {
        printf("connect error\n");
        return -1;
    }
#endif

    while(1)
    {
        if(fgets(SendBuff, 1023, stdin))
        {
#if 0
            SendLen = send(SocketClient, SendBuff, strlen(SendBuff), 0);
#else
            AddrLen = sizeof(struct sockaddr);
            SendLen=  sendto(SocketClient, SendBuff, 1023, 0,(const struct sockaddr *)&SocketServerAddr, AddrLen);
#endif
            if(SendLen <= 0)
            {
                close(SocketClient);
                return -1;
            }
        }
    }

    return 0;
}