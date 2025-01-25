/*
 * @Author: xubao xubaoyongganfei@gmail.com
 * @Date: 2025-01-25 21:37:59
 * @LastEditors: xubao xubaoyongganfei@gmail.com
 * @LastEditTime: 2025-01-25 23:02:18
 * @FilePath: /gateway/app/app_mqtt.c
 * @Description: 这是默认设置,请设置`customMade`, 打开koroFileHeader查看配置 进行设置: https://github.com/OBKoro1/koro1FileHeader/wiki/%E9%85%8D%E7%BD%AE
 */
#include "app_mqtt.h"
#include "thirdparty/log.c/log.h"
#include <assert.h>
#include <stdlib.h>
#include <MQTTClient.h>
//库封装客户端
static MQTTClient client;
static MQTTClient_connectOptions conn_opts=MQTTClient_connectOptions_initializer;
static int (*recv_callback)(char *,int);
/// @brief 连接丢失时回调函数
/// @param context  
/// @param cause 
void app_mqtt_connectionLost(void *context,char *cause){
    assert(context==NULL);
    log_fatal("Connection lost:%s",cause);
    exit(EXIT_FAILURE);
}

/// @brief 连接到达时的回调函数
/// @param context 
/// @param topicName 
/// @param topicLen 
/// @param message 
/// @return 
int app_mqtt_messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *message){
    assert(context==NULL);
    //处理回调
    log_trace("Message arrived\n\ttopic");
    recv_callback((char *)message->payload,message->payloadlen);
}   
//数据发送完毕回调函数
 void app_mqtt_deliveryComplete(void *context, MQTTClient_deliveryToken dt){
    assert(context==NULL);
    log_trace("Message with token %d delivered",dt);
 }


//连接操作一些选项
//官网文档https://eclipse-paho.github.io/paho.mqtt.c/MQTTClient/html/_m_q_t_t_client_8h.html#a7649e3913f9a216424d296f88a969c59
MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
int app_mqtt_init()
{
    //初始化客户端
    int rc;
    rc=MQTTClient_create(&client,MQTT_SERVER,CLIENT_ID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc!=MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_create error:%d",rc);
        goto EXIT;
    }
    //设置回调函数
    rc=MQTTClient_setCallbacks(client,NULL,app_mqtt_connectionLost,app_mqtt_messageArrived,app_mqtt_deliveryComplete);
    if (rc!=MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_setCallbacks error:%d",rc);
        goto DESTORY_EXIT;
    }
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    rc=MQTTClient_connect(client, &conn_opts);
    if (rc!=MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_connect error:%d",rc);
        goto DESTORY_EXIT;
    }
    //订阅话题
    rc = MQTTClient_subscribe(client, PULL_TOPIC, 0);
    if (rc!=MQTTCLIENT_SUCCESS)
    {
        log_error("MQTTClient_subscribe error:%d",rc);
        goto DISCONNECT_EXIT;
    }
    log_info("MQTTClient initialize success");
    return 0;
//goto语句
DISCONNECT_EXIT:
    MQTTClient_disconnect(client,1000);
DESTORY_EXIT:
    MQTTClient_destroy(&client);
EXIT:
    return -1;
}

void app_mqtt_registerRecvCallback(int (*callback)(char *, int))
{
    recv_callback=callback;
}

void app_mqtt_send(char *json_str, int len)
{
    
}

void app_mqtt_close()
{
    log_info("MQTT Client Closong......");
    MQTTClient_disconnect(client,1000);
    MQTTClient_destroy(&client);
}
