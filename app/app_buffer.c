#include "app_buffer.h"
#include <string.h>
#include <stdlib.h>
#include "thirdparty/log.c/log.h"
//互斥锁初始化为默认状态（即未加锁状态）。
static pthread_mutex_t lock_initializer = PTHREAD_MUTEX_INITIALIZER;
//初始化
int app_buffer_init(Buffer *buffer,int size)
{

    //申请内存
    buffer->ptr=malloc(size);
    if (!buffer->ptr)
    {
        log_warn("Not enough memory for Buffer%p",buffer);
        return -1;
    }
    memcpy(&buffer->lock,&lock_initializer,sizeof(pthread_mutex_t));
    buffer->size=size;
    buffer->start=0;
    buffer->len=0;
    log_debug("Buffer %p create",buffer);
    return 0;
}
//读
int app_buffer_read(Buffer *buffer, void *buf, int len)
{
       if (!buffer || !buf)
        {
            log_warn("buffer和buf不能为空");
            return -1;
        }
    //加锁
    pthread_mutex_lock(&buffer->lock);
    //首先确定实际读取的数据长度为多少
    if (len>buffer->len)
    {
        len=buffer->len;
    }
    if (len==0)
    {
        pthread_mutex_unlock(&buffer->lock);
        return 0;
    }    
    //再次判断是否能够一次读取
    if (buffer->start+len<=buffer->size)
    {   
        //一次可以读完
        memcpy(buf,buffer->ptr+buffer->start,len);
        buffer->start+=len;
    }else{
        //一次读不完
        //第一次能够读取的长度为
        int first_len=buffer->size-buffer->start;
        memcpy(buf,buffer->ptr+buffer->start,first_len);
        //第二次从buf+first_len开始拷贝
        memcpy(buf+first_len,buffer->ptr,len-first_len);
        buffer->start=len-first_len; 
    }
    buffer->len-=len;
    pthread_mutex_unlock(&buffer->lock);
    log_trace("Buffer status after read:start %d,len %d",buffer->start,buffer->len);
    return 0;
}
//写
int app_buffer_write(Buffer *buffer, void *buf, int len)
{
     if (!buffer || !buf)
        {
            log_warn("buffer和buf不能为空");
            return -1;
        }
    pthread_mutex_lock(&buffer->lock);
    //判断数据能否写入
    if (len>buffer->size-buffer->len)
    {
        //剩余空间不够
        pthread_mutex_unlock(&buffer->lock);
        log_warn("Buffer %p is not enougt",buffer);
        return -1;
    }
    //分一次写还是多次写
    //找到写入的起点
    //读取的起点+数据长度=写入的起点
    int write_offset=buffer->start+buffer->len; 

    if (write_offset>buffer->size)
    {
       write_offset-=buffer->size;
    }
    //判断写入起点到尾部还有多少空间
    if (write_offset+len<=buffer->size)
    {
        //一次写入
        memcpy(buffer->ptr+write_offset,buf,len);

    }else{
        //两次写入
        int first_len =buffer->size-write_offset;
        memcpy(buffer->ptr+write_offset,buf,first_len);
        memcpy(buffer->ptr,buf+first_len,len-first_len);
    }
    buffer->len+=len;
    pthread_mutex_unlock(&buffer->lock);
    log_trace("Buffer status after write:start %d len %d",buffer->start,buffer->len);
    return 0;
}

void app_buffer_close(Buffer *buffer)
{
    if (buffer->ptr)
    {
        free(buffer->ptr);
        buffer->ptr=NULL;
    }
    buffer->size=0;
    buffer->len=0;
}
