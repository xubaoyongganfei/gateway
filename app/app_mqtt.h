/*
 * @Author: xubao xubaoyongganfei@gmail.com
 * @Date: 2025-01-25 21:38:04
 * @LastEditors: xubao xubaoyongganfei@gmail.com
 * @LastEditTime: 2025-01-25 22:53:04
 * @FilePath: /gateway/app/app_mqtt.h
 */
#ifndef __APP_MQTT_H
#define __APP_MQTT_H
#define MQTT_SERVER "tcp://192.168.200:1833"
#define CLIENT_ID "xjl-ubuntu-12bca47b0bc1"
#define PULL_TOPIC "TEST_PULL"
#define PUSH_TOPIC "TEST_PUSH"
/// @brief MQTT初始化
/// @return int 0:成功 -1：失败
int app_mqtt_init();
 
/// @brief MQTT注册接收回调函数
/// @param recv_callback 接收回调函数
void app_mqtt_registerRecvCallback(int(*callback)(char *ptr,int));

/// @brief MQTT发送数据
/// @param json_str  json字符串
/// @param len json字符串长度
void app_mqtt_send(char *json_str,int len);

/// @brief MQTT关闭
void app_mqtt_close();

#endif