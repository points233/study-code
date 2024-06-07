#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static char g_buf[1000];
static int g_hasData = 0;

static void *my_thread_func(void *data)
{
    while(1)
    {
        /* 等待通知 */
        while(g_hasData == 0);

        /* 打印 */
        printf("recv: %s\n", g_buf);
        g_hasData = 0;
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int ret;
    pthread_t tid;
    /* 创建接收线程 */
    ret = pthread_create(&tid, NULL, my_thread_func, NULL);
    if(ret)
    {
        printf("pthread_create error!\n");
        return -1;
    }

    /* 主线程读取标准驶入，发给接收线程 */
    while(1)
    {
        fgets(g_buf, 1000, stdin);

        /* 通知接收线程 */
        g_hasData = 1;
    }
    return 0;
}