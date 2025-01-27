#ifndef __APP_TASK_H
#define __APP_TASK_H
typedef void (*Task)(void *args);

int app_task_init(int executors);

int app_task_register(Task task,void *args);

void app_task_wait(void);

void app_task_close();
#endif