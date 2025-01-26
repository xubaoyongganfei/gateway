#include "app_message.h"
#include <string.h>
#include "thirdparty/log.c/log.h"
#include "thirdparty/cJSON/cJSON.h" 
#include "stdlib.h"
//二进制转字符串  调用者需要负责释放str
static char *bin_to_str(unsigned char *binary,int len){
    char* hex_str=malloc(len*2+1);
    if (!hex_str)
    {
        log_warn("Nit enough for message");
        return NULL;
    }
    
    for (int i = 0; i < len; i++)
    {
        sprintf(hex_str+i*2,"%02X",binary[i]);
    }
    hex_str[len*2]='\0';
    return hex_str;
}

//字符串转换成二进制 返回实际转换=长度
static int  str_to_bin(char *hex_str,unsigned char *binary,int len){
    if (strlen(hex_str)%2!=0)
    {
       log_warn("Hex string is not valid");
        return -1;
    }
    
    if (len<(int)(strlen(hex_str)/2))
    {
        log_warn("Buffer len is not enough");
        return -1;
    }
    len=strlen(hex_str)/2;
     for (int i = 0; i < len; i++)
    {
        // 现在需要转化的字节有两个，分别是hex_str[i*2]和hex_str[i*2+1]
        char high = hex_str[i * 2];
        char low = hex_str[i * 2 + 1];

        // 首先处理高位
        if (high <= '9' && high >= '0')
        {
            binary[i] = high - '0';
        }
        else if (high >= 'a' && high <= 'f')
        {
            binary[i] = high - 'a' + 10;
        }
        else if (high >= 'A' && high <= 'F')
        {
            binary[i] = high - 'A' + 10;
        }
        binary[i] <<= 4;

        // 再处理低位
        if (low <= '9' && low >= '0')
        {
            binary[i] |= low - '0';
        }
        else if (low >= 'a' && low <= 'f')
        {
            binary[i] |= low - 'a' + 10;
        }
        else if (low >= 'A' && low <= 'F')
        {
            binary[i] |= low - 'A' + 10;
        }
    }
    return len;
}



int app_message_initBinary(Message *message, void *binary, int len)
{
    //首先初始化message
    memset(message,0,sizeof(Message));
    //首先读取类型和长度
    memcpy(&message->connection_type,binary,1);
    memcpy(&message->id_len,binary+1,1);
    memcpy(&message->data_len,binary+2,1);
    if (len!=message->id_len+message->data_len+3)
    {
        log_warn("Meaasge is not valid");
        return -1;
    }
    //数据拷贝
    message->payload=malloc(message->id_len+message->data_len);
    if (!message->payload)
    {
       log_warn("nit enough for message");
       return -1;
    }
    //申请成功
    memcpy(message->payload,binary+3,message->id_len+message->data_len);
    return 0;
}

int app_message_initByJson(Message *message, char *json_str, int len)
{
    //使用吃JSON解析字符串
    cJSON *json_object=cJSON_ParseWithLength(json_str,len);
    //首先读取ConnectionType
    cJSON *connection_type= cJSON_GetObjectItem(json_object,"connectoon_type");
    message->connection_type=connection_type->valueint;
    //读取id和data长度
    cJSON *id=cJSON_GetObjectItem(json_object,"id");
    if (strlen(id->valuestring)/2!=0)
    {
        log_warn("JSON is not valid");
        return -1;
    }
    message->id_len=strlen(id->valuestring)/2;

    cJSON *data=cJSON_GetObjectItem(json_object,"data");
    if (strlen(data->valuestring)%2!=0)
    {
        log_warn("JSON is not valid");
        return -1;
    }
    message->data_len=strlen(data->valuestring)/2;
    //申请message的payload的内存
    message->payload=malloc(message->id_len+message->data_len);
    if (!message->payload)
    {
        log_warn("Not enough for message");
        //free(message->payload);
        return -1;
    }
    if (str_to_bin(id->valuestring,message->payload,message->id_len)<0)
    {
        log_warn("Convertion failed");
        free(message->payload);
        return -1;
    }
    if (str_to_bin(data->valuestring,message->payload+message->id_len,message->data_len)<0)
    {
         log_warn("Convertion failed");
         free(message->payload);
        return -1;
    }
    cJSON_Delete(json_object);
    return 0;
}

int app_message_saveBinary(Message *message, void *binary, int len)
{
    int total_len=message->id_len+message->data_len+3;
    //将来需要保存二进制需要的长度为3个字节的元数据和id_len+data_len长度数据
    if (len<total_len)
    {
        log_warn("Buff not enough");
        return -1;
    }
    memcpy(binary,&message->connection_type,1);
    memcpy(binary+1,&message->id_len,1);
    memcpy(binary+2,&message->data_len,1);
    memcpy(binary+3,&message->payload,message->id_len+message->data_len);
    return total_len;
}

int app_message_saveJson(Message *message, char *json_str, int len)
{
    cJSON *cjson_object=cJSON_CreateObject();
    cJSON_AddNumberToObject(cjson_object,"connection_type",message->connection_type);
    cJSON_AddStringToObject(cjson_object, "id", bin_to_str(message->payload, message->id_len));
    cJSON_AddStringToObject(cjson_object, "data", bin_to_str(message->payload + message->id_len, message->data_len));

    char *str=cJSON_PrintUnformatted(cjson_object);
    if (len<(int)(strlen(str)+1))
    {
        log_warn("Buffer not enough for message");
        return -1;
    }
    strcpy(json_str,str);
    cJSON_free(str);
    cJSON_Delete(cjson_object);
    return 0;
}

void app_message_free(Message *message)
{
    if (message->payload)
    {
        free(message->payload);
        message->payload=NULL;
    }
    
}
