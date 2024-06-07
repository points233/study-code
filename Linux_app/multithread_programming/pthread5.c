#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>

static char g_buf[1000];
static sem_t g_sem;
static pthread_mutex_t g_tMutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_tConVar = PTHREAD_COND_INITIALIZER;

static void *my_thread_func(void *data)
{
    while(1)
    {
        /* 打印 */
        pthread_mutex_lock(&g_tMutex);
        pthread_cond_wait(&g_tConVar, &g_tMutex); /* 等待通知 */
        printf("recv: %s\n", g_buf);
        pthread_mutex_unlock(&g_tMutex);
    }
    return NULL;
}

int main(int argc, char **argv)
{
    int ret;
    pthread_t tid;
    char buf[1000];

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
        fgets(buf, 1000, stdin);
        pthread_mutex_lock(&g_tMutex);
        memcpy(g_buf, buf, 1000);
        pthread_cond_signal(&g_tConVar); /* 通知接收线程 */
        pthread_mutex_unlock(&g_tMutex);
    }
    return 0;
}