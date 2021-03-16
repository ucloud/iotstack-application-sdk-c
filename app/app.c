#include "app.h"

natsConnection *conn = NULL;
natsSubscription *sub = NULL;
static cJSON *app_config_json = NULL;
char *app_name = NULL;
char *app_productSN = NULL;
char *app_deviceSN = NULL;
char *app_info = NULL;
msg_handler normal_cb = NULL;
msg_handler rrpc_cb   = NULL;


char edge_router_subject[NATS_SUBJECT_MAX_LEN] = {0};

app_status nats_subscribe(const char *subject, natsMsgHandler cb, void *cbClosure)
{
    app_status status = APP_OK;    
    
    natsConnection_Subscribe(&sub, conn, subject, cb, cbClosure);
    natsConnection_Flush(conn);
    return status;
}

app_status nats_publish(const char *topic, const char *str)
{
    app_status status = APP_OK;

    natsConnection_PublishString(conn, topic, str);
    natsConnection_Flush(conn);

    return status;
}

void log_print(const char *format,...)
{
    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    fflush(stdout);
    return;
}

void log_write(log_level level, const char *format,...)
{    
    if((level < LOG_DEBUG) || (level > LOG_ERROR))
    {
        return;
    }

    const char *log_lev[4] = {"debug", "info", "warn", "error"};
    struct timeval stamp;
    char *msg_str_rep = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == msg_str_rep)
    {
        return;
    }
    memset(msg_str_rep, 0, NATS_MSG_MAX_LEN);
    
    gettimeofday(&stamp, NULL);
    char *msg_str = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == msg_str)
    {
        APP_FREE(msg_str_rep);
        return;
    }
    memset(msg_str, 0, NATS_MSG_MAX_LEN);

    char *log_msg = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == log_msg)
    {
        APP_FREE(msg_str_rep);
        APP_FREE(msg_str);
        return;
    }
    memset(log_msg, 0, NATS_MSG_MAX_LEN);
    va_list args;

    va_start(args, format);
    vsnprintf(msg_str, NATS_MSG_MAX_LEN, format, args);
    va_end(args);

    // 将json字段转换成字符串
    replace_str(msg_str_rep, msg_str, "\"", "\\\"");
    snprintf(log_msg, NATS_MSG_MAX_LEN, LOG_UPLOAD_FORMAT, app_get_name(), log_lev[level], msg_str_rep, stamp.tv_sec);
    
    nats_publish(EDGE_LOG_UPLOAD_SUBJECT, log_msg);

    APP_FREE(msg_str_rep);
    APP_FREE(msg_str);
    APP_FREE(log_msg);
    return;
}


static void _parse_config(void)
{
    if(NULL == app_config_json)
    {
        unsigned int file_len = 0;
        FILE *fp;

        file_len = calc_file_len(CONFIG_FILE_PATH);
        char *driver_config_str_tmp = (char *)APP_MALLOC(file_len + 1);
        if(NULL == driver_config_str_tmp)
        {
            log_write(LOG_ERROR, "file_str malloc fail!");
            return;
        }
        memset(driver_config_str_tmp, 0, file_len + 1);
        
        fp = fopen(CONFIG_FILE_PATH, "r");
        if(NULL == fp)
        {
            log_write(LOG_ERROR, "cannot open file:%s", CONFIG_FILE_PATH);
            return;
        }
        if(file_len != fread(driver_config_str_tmp, 1, file_len, fp))
        {
            log_write(LOG_ERROR, "fread file:%s fail!", CONFIG_FILE_PATH);
        }
        driver_config_str_tmp[file_len] = '\0';
        fclose(fp);
        
        log_print("driver_config_str_tmp:%s",driver_config_str_tmp);
        app_config_json = cJSON_Parse(driver_config_str_tmp);
        APP_FREE(driver_config_str_tmp);

        /* parse appName */
        cJSON *app_name_json = cJSON_GetObjectItem(app_config_json, "appName");
        if(NULL == app_name_json)
        {
            log_write(LOG_ERROR, "parse appName fail!");
            return;
        }        
        int app_name_len = strlen(app_name_json->valuestring);
        app_name = (char *)APP_MALLOC(app_name_len+1);
        strncpy(app_name, app_name_json->valuestring, app_name_len);
        app_name[app_name_len]='\0';

        
        /* parse productSN */
        cJSON *productSN_json = cJSON_GetObjectItem(app_config_json, "productSN");
        if(NULL == productSN_json)
        {
            log_write(LOG_ERROR, "parse productSN fail!");
            return;
        }
        int app_productSN_len = strlen(productSN_json->valuestring);
        app_productSN = (char *)APP_MALLOC(app_productSN_len+1);
        strncpy(app_productSN, productSN_json->valuestring, app_productSN_len);
        app_productSN[app_productSN_len]='\0';
        
        /* parse deviceSN */
        cJSON *deviceSN_json = cJSON_GetObjectItem(app_config_json, "deviceSN");
        if(NULL == deviceSN_json)
        {
            log_write(LOG_ERROR, "parse deviceSN fail!");
            return;
        }
        int app_deviceSN_len = strlen(deviceSN_json->valuestring);
        app_deviceSN = (char *)APP_MALLOC(app_deviceSN_len+1);
        strncpy(app_deviceSN, deviceSN_json->valuestring, app_deviceSN_len);
        app_deviceSN[app_deviceSN_len]='\0';

        /* parse appInfo */
        cJSON *appInfo_json = cJSON_GetObjectItem(app_config_json, "appInfo");
        if(NULL == appInfo_json)
        {
            log_write(LOG_ERROR, "parse appInfo fail!");
            return;
        }
        int app_info_len = strlen(cJSON_PrintUnformatted(appInfo_json));
        app_info = (char *)APP_MALLOC(app_info_len+1);
        strncpy(app_info, cJSON_PrintUnformatted(appInfo_json), app_info_len);
        app_info[app_info_len]='\0';     
    }
    return;
}

char *app_get_name()
{
    if(NULL == app_name)
        return NULL;
    return app_name;
}

char *app_get_productSN()
{
    if(NULL == app_productSN)
        return NULL;
    return app_productSN;
}

char *app_get_deviceSN()
{
    if(NULL == app_deviceSN)
        return NULL;
    return app_deviceSN;
}

char *app_get_info()
{
    if(NULL == app_info)
        return NULL;
    return app_info;
}

static app_status app_rrpc_check(char *topic)
{
    if(strstr(topic,"/rrpc/request/") == NULL){
        return APP_ERR; 
    }else{
        return APP_OK;
    }
}


static void _handle_message(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure)
{
    log_write(LOG_DEBUG, "Received msg: %s - %.*s", natsMsg_GetSubject(msg), natsMsg_GetDataLength(msg), natsMsg_GetData(msg));
    
    cJSON *msg_json = cJSON_Parse(natsMsg_GetData(msg));
    if (!msg_json) 
    {
        log_write(LOG_ERROR, "_handle_message json parse error: [%s]",cJSON_GetErrorPtr());
        goto end;
    }
    cJSON *topic = cJSON_GetObjectItem(msg_json, "topic");
    if(NULL == topic)
    {
        log_write(LOG_ERROR, "cannot find topic, illegal msg");
        cJSON_Delete(msg_json);
        goto end;
    }
    log_write(LOG_DEBUG, "_handle_message topic:%s", topic->valuestring);
        
    cJSON *msg_base64code = cJSON_GetObjectItem(msg_json, "payload");
    if(NULL == msg_base64code)
    {
        log_write(LOG_ERROR, "cannot find payload, illegal msg");   
        cJSON_Delete(msg_json);
        goto end;
    }
    log_write(LOG_DEBUG, "_handle_message msg_base64code:%s", msg_base64code->valuestring);
    char *msg_base64decode = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == msg_base64decode)
    {
        log_write(LOG_ERROR, "msg_base64decode malloc fail!");
        cJSON_Delete(msg_json);
        goto end;
    }
    memset(msg_base64decode, 0, NATS_MSG_MAX_LEN);
    int msg_base64decodeLen = base64_decode(msg_base64code->valuestring, strlen(msg_base64code->valuestring), msg_base64decode);
    log_write(LOG_DEBUG, "_handle_message msg_base64decode:%s", msg_base64decode);

    if(NULL != rrpc_cb){
        if(app_rrpc_check(topic->valuestring) == APP_OK){
            rrpc_cb(topic->valuestring, msg_base64decode, msg_base64decodeLen);
            APP_FREE(msg_base64decode);
            goto end;
        }
    }

    if(NULL != normal_cb){
        normal_cb(topic->valuestring, msg_base64decode, msg_base64decodeLen);
    }
    APP_FREE(msg_base64decode);
end:
    natsMsg_Destroy(msg);
    cJSON_Delete(msg_json);
    return;
}

app_status app_register_cb(msg_handler normal_handler, msg_handler rrpc_handler)
{
    app_status status = APP_OK;
    char edge_app_subject[NATS_SUBJECT_MAX_LEN] = {0};
    if(normal_handler != NULL)
        normal_cb = normal_handler;
    rrpc_cb = rrpc_handler;

    snprintf(edge_app_subject, NATS_SUBJECT_MAX_LEN, EDGE_APP_SUBJECT_FORMAT, app_get_name());
    log_write(LOG_DEBUG, "edge_app_subject:%s",edge_app_subject);

    status = nats_subscribe(edge_app_subject, _handle_message, NULL);
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "edge_subscribe %s fail! status:%d", edge_app_subject, status);
        return APP_PROTOCOL_ERROR;
    }
    return status;
}

app_status app_publish(const char *topic, const char *data, int dataLen)
{
    app_status status = APP_OK;

    if((NULL == topic) || (NULL == data))
    {
        return APP_INVALID_ARG;
    }

    char *normal_payload_base64 = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == normal_payload_base64)
    {
        log_write(LOG_ERROR, "normal_payload_base64 malloc fail!");
        return APP_NO_MEMORY;
    }
    memset(normal_payload_base64, 0, NATS_MSG_MAX_LEN);
    base64_encode(data, dataLen, normal_payload_base64);
    log_write(LOG_DEBUG, "send data:%s",data);

    char *normal_msg = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == normal_msg)
    {
        log_write(LOG_ERROR, "normal_msg malloc fail!");
        APP_FREE(normal_payload_base64);
        return APP_NO_MEMORY;
    }
    memset(normal_msg, 0, NATS_MSG_MAX_LEN);
    snprintf(normal_msg, NATS_MSG_MAX_LEN, NORMAL_MSG_FORMAT, topic, normal_payload_base64, app_get_name());

    status = nats_publish(edge_router_subject, normal_msg);
    
    APP_FREE(normal_payload_base64);    
    APP_FREE(normal_msg);
    return status;
}

app_status app_publishString(const char *topic, const char *str)
{
    app_status status;

    status = app_publish(topic,str,strlen(str));

    return status;
}


static void _handle_status_sync(union sigval v)
{
    app_status status = APP_OK;
    char *sync_msg = (char *)APP_MALLOC(NATS_MSG_MAX_LEN);
    if(NULL == sync_msg)
    {
        log_write(LOG_ERROR, "sync_msg malloc fail!");
        return;
    }
    
    memset(sync_msg, 0, NATS_MSG_MAX_LEN);
    snprintf(sync_msg, NATS_MSG_MAX_LEN, STATUS_SYNC_FORMAT, app_get_name());
    log_write(LOG_DEBUG, "sync_msg:%s",sync_msg);
    status = nats_publish(EDGE_APP_STATUS_SUBJECT, sync_msg);
    if(APP_OK != status)
    {
        log_write(LOG_ERROR, "publish sync msg fail!");
        APP_FREE(sync_msg);
        return;
    }
    
    APP_FREE(sync_msg);
    return;
}


static void _fetch_online_status(void)
{
    struct sigevent evp;
    struct itimerspec ts;
    timer_t timer;
    int ret;

    memset(&evp, 0, sizeof(evp));
    evp.sigev_value.sival_ptr = &timer;        
    evp.sigev_notify = SIGEV_THREAD;  
    evp.sigev_notify_function = _handle_status_sync;  
    evp.sigev_value.sival_int = 0;
    
    ret = timer_create(CLOCK_REALTIME, &evp, &timer);
    if(ret)
        perror("timer_create");

    ts.it_interval.tv_sec = 15;
    ts.it_interval.tv_nsec = 0;
    ts.it_value.tv_sec = 15;
    ts.it_value.tv_nsec = 0;

    ret = timer_settime(timer, 0, &ts, NULL);
    if( ret )
        perror("timer_settime");    
    return;
}

app_status app_common_init(void)
{
    app_status status = APP_OK;
    char *nats_server_url = NULL;
    
    // nats server取IOTEDGE_NATS_ADDRESS环境或者本地host
    nats_server_url =  getenv("IOTEDGE_NATS_ADDRESS");
    status = natsConnection_ConnectTo(&conn, nats_server_url != NULL?nats_server_url:NATS_SERVER_DEFAULT_URL);
    if(APP_OK != status)
    {
        log_print("connect nats fail!\r\n");
        return APP_PROTOCOL_ERROR;
    }

    _parse_config();

    snprintf(edge_router_subject, NATS_SUBJECT_MAX_LEN, EDGE_ROUTER_SUBJECT_FORMAT, app_get_name());
    log_print(LOG_DEBUG, "edge_router_subject:%s",edge_router_subject);

    _fetch_online_status();

    return APP_OK;
}

app_status app_rrpc_response(char *topic,char *payload, int payloadLen)
{
    app_status status; 
    char response_topic[128];
    replace_str(response_topic, topic, "request", "response");
    status = app_publish(response_topic, payload, payloadLen);
    if(APP_OK != status){
        log_write(LOG_ERROR, "app_publish rrpc fail");
        return APP_ERR;
    }
    log_write(LOG_DEBUG, "app_publish rrpc :%s",payload);
    return APP_OK;
}






