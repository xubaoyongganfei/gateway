/*
 * @Author: xubao xubaoyongganfei@gmail.com
 * @Date: 2025-01-25 17:20:51
 * @LastEditors: xubao xubaoyongganfei@gmail.com
 * @LastEditTime: 2025-01-25 17:49:00
 * @FilePath: /gateway/app/app_buffer.h
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#ifndef __APP_BUFFER_H
#define __APP_BUFFER_H
#include <pthread.h>
typedef struct BufferStruct{
    void *ptr;   //缓存区指针
    int size;   //缓存区总长度
    int start;  //数据起始offset
    int len;    //数据长度
    pthread_mutex_t lock;  //Buffer锁
}Buffer;
//Buffer操作方法

/// @brief 初始化Buffer
/// @param buffer buffer对象指针
///@param size buffer大小
/// @return //int 0成功 1失败
int app_buffer_init(Buffer *buffer,int size);

/// @brief 从Buffer中读取数据
/// @param buffer  buffer对象指针
/// @param buf //读取数据的缓存区指针
/// @param len 缓存区长度
/// @return int 返回实际读取的长度 -1失败
int app_buffer_read(Buffer *buffer,void *buf,int len);

/// @brief 从Buffer中写入数据
/// @param buffer 要写入的数据指针
/// @param buf  要写入的长度
/// @param len  0成功 -1失败
/// @return 
int app_buffer_write(Buffer *buffer,void *buf,int len);

/// @brief 销毁buffer
/// @param buffer buffer指针
void app_buffer_close(Buffer *buffer);
#endif