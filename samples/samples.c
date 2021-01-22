#include "app.h"

void recvmsg_handler(char *topic, char *payload)
{
    log_write(LOG_INFO, "receive topic:%s",topic);
    log_write(LOG_INFO, "receive payload:%s",payload);
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
        goto end;
    }

    log_write(LOG_INFO, "productSN:%s, deviceSN:%s",app_get_productSN(),app_get_deviceSN());
    log_write(LOG_INFO, "app info:%s",app_get_info());

    //注册回调函数
    status = app_register_cb(recvmsg_handler);
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "app_register_cb fail");
        goto end;
    }

    //获取应用配置信息
    app_info = cJSON_Parse(app_get_info());
    if (!app_info) 
    {
        log_write(LOG_ERROR, "parse app info fail");
        goto end;
    }
    /*
        "topic":"/%s/%s/upload"
    */
    cJSON *topic_format = cJSON_GetObjectItem(app_info, "topic");

    snprintf(topic_str, 100, topic_format->valuestring, app_get_productSN(), app_get_deviceSN());
    while(1)
    {    
        sleep(5);
        gettimeofday(&stamp, NULL);
        memset(time_stamp, 0, 100);
        snprintf(time_stamp, 100, "{\"timestamp\": \"%ld\"}", stamp.tv_sec);
        log_write(LOG_INFO, "send message[%s]", time_stamp);
        
        status = app_publish(topic_str, time_stamp);
        if(APP_OK != status)
        {
            log_write(LOG_ERROR, "app_publish fail");
            goto end;
        }
    }

end:    
    cJSON_Delete(app_info);
    return status;
}

