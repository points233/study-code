#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

static void *my_thread_func(void *data)
{
    while(1)
    {
        sleep(1);
    }
    return 0;
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
        sleep(1);
    }
    return 0;
}