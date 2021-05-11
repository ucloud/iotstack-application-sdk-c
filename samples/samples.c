#include "app.h"
#define DEFAULT_TOPIC_FMT "/%s/%s/upload"
void app_normal_msg_handler(char *topic, char *payload, int payloadLen)
{
    log_write(LOG_INFO, "receive topic:%s",topic);
    log_write(LOG_INFO, "receive payload:%s payloadLen:%d",payload, payloadLen);
    return;
}

void app_rrpc_msg_handler(char *topic, char *payload, int payloadLen)
{
    log_write(LOG_INFO, "rrpc topic:%s",topic);
    log_write(LOG_INFO, "rrpc payload:%s payloadLen:%d",payload, payloadLen);
    char *response_str = "rrpc response sample!";
    app_rrpc_response(topic, response_str, strlen(response_str));
    return;
}

void app_nats_msg_handler_user(char *topic, char *payload, int payloadLen)
{
    log_write(LOG_INFO, "topic:%s payload:%s payloadLen:%d", topic, payload, payloadLen);
    return;
}

int main(int argc, char **argv)
{
    app_status status = APP_OK;    
    char topic_str[100] = {0};    
    struct timeval stamp;
    char time_stamp[100] = {0};
    cJSON *app_info = NULL;
    //初始化，获取产品、设备SN号信息
    status = app_common_init();
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "app_common_init fail");
        return APP_ERR;
    }

    log_write(LOG_INFO, "productSN:%s, deviceSN:%s",app_get_productSN(),app_get_deviceSN());
    log_write(LOG_INFO, "app info:%s",app_get_info());

    //注册回调函数
    status = app_register_cb(app_normal_msg_handler, app_rrpc_msg_handler, app_nats_msg_handler_user);
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "app_register_cb fail");
        return APP_ERR;
    }

    //获取应用配置信息
    app_info = cJSON_Parse(app_get_info());
    if (!app_info) 
    {
        log_write(LOG_ERROR, "parse app info fail");
        return APP_ERR;
    }

    /*
        "topic":"/%s/%s/upload"
    */
    cJSON *topic_format = cJSON_GetObjectItem(app_info, "topic");
    if(topic_format ==  NULL){
        snprintf(topic_str, 100, DEFAULT_TOPIC_FMT, app_get_productSN(), app_get_deviceSN());
    }else{
        snprintf(topic_str, 100, topic_format->valuestring, app_get_productSN(), app_get_deviceSN());
    }

    //subscribe nats subject
    status = nats_subscribe("/a/b/c");
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "nats_subscribe nats subject fail");
        return APP_ERR;
    }

    while(1)
    {    
        sleep(5);
        gettimeofday(&stamp, NULL);
        memset(time_stamp, 0, 100);
        snprintf(time_stamp, 100, "{\"timestamp\": \"%ld\"}", stamp.tv_sec);
        log_write(LOG_INFO, "send message[%s]", time_stamp);
        
        status = app_publishString(topic_str, time_stamp);
        status |= app_publish(topic_str, "0D0A2131", 8);
        if(APP_OK != status)
        {
            log_write(LOG_ERROR, "app_publish fail");
            goto end;
        }
        //publish msg to subscribed nats subject
        status = nats_publish("/a/b/c",time_stamp,strlen(time_stamp));
        if(APP_OK != status)
        {
            log_write(LOG_ERROR, "edge_publish nats subject fail");
            goto end;
        }
    }

end:    
    cJSON_Delete(app_info);
    return status;
}

