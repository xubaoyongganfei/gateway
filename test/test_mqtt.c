#include "app/app_mqtt.h"
#include <assert.h>
#include <string.h>
#include <unistd.h>

//标准
static int receive_flag = 0;
int recvCallback(char *str,int len){
    receive_flag=1;
    assert(memcmp(str,"Hello world!",len)==0);   
    return 0;
} 

int main()
{
    app_mqtt_init();
    app_mqtt_registerRecvCallback(recvCallback);
    app_mqtt_send("Hello world!",12);
    sleep(1);
    assert(receive_flag==1);
    app_mqtt_close();
    return 0;
}
