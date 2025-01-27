#include "app_task.h"
//消息队列
#include <mqueue.h>
//多线程
#include <pthread.h>
#include <sys/types.h>
#include <string.h>
#include "thirdparty/log.c/log.h"
#include <stdlib.h>
#define MSG_LEN sizeof(struct TaskStruct)
struct  TaskStruct
{
   Task task;
   void* argv;
};
static pthread_t *executors_ptr;
static int executors_count;
//消息队列
static mqd_t mq;
static void *app_task_executor(void *argv){
    int count=argv;    
    log_info("Executor %d start!",count);
    struct TaskStruct task_struct;
    while (1)
    {
        if (mq_receive(mq,(char *)&task_struct,MSG_LEN,0)<0)
        {
           continue;
        }
        
        //执行任务
        task_struct.task(task_struct.argv);
    }
    return NULL;
}
int app_task_init(int executors)
{
    //首先开启mq
    struct mq_attr attr;
    attr.mq_maxmsg=10;
    attr.mq_msgsize=MSG_LEN;
    mq=mq_open("/gateway-mqueu",O_RDWR|O_CREAT,0644,&attr);
    if (mq<0)
    {
        log_error("mq_open error");
        goto EXIT;
    }
    //启动后台线程
    executors_count=executors;
    executors_ptr=malloc(executors_count *sizeof(pthread_t));
    if (!executors_ptr)
    {
        log_error("Not enough memory for task manger");
        goto MQ_EXIT; 
    }
    memset(executors_ptr,0,executors_count *sizeof(pthread_t));
    int i;
    for ( i = 0; i < executors_count; i++)
    {
         if (pthread_create(executors_ptr+i,NULL,app_task_executor,(void *)i)<0)
         {
           goto FREE_EXIT;
         }
    }  
    log_info("task manager started");
    return 0;
FREE_EXIT:
    for ( i = 0; i < executors_count; i++)
    {
        if (executors_ptr[i])
        {
           pthread_cancel(executors_ptr[i]);
           pthread_join(executors_ptr[i],NULL);
        }
        
    }
    free(executors_ptr);
MQ_EXIT:
    mq_unlink("/gateway-mqueu");
EXIT:
    return -1;
}

int app_task_register(Task task, void *args)
{
    struct TaskStruct task_struct={
        .task=task,
        .argv=args,
    };
    int result=mq_send(mq,(char *)&task_struct,MSG_LEN,0);
    log_debug("Task %p registered",task);
    return result;
}

void app_task_wait(void)
{

     for (int i = 0; i < executors_count; i++)
    {
        if (executors_ptr[i])
        {
           pthread_join(executors_ptr[i],NULL);
        }
        
    }
    free(executors_ptr);
    mq_unlink("./gateway-mqueu");
    log_info("Task manager closed");
}

void app_task_close()
{
    log_info("Closing task manager...");
     for (int i = 0; i < executors_count; i++)
    {
        if (executors_ptr[i])
        {
           pthread_cancel(executors_ptr[i]);
        }
        
    }
}
