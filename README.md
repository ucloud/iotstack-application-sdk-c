## 编译
编译依赖：cmake 2.8（依赖的nats库仅支持cmake编译）
```
sudo apt-get install cmake
```
编译方式：GNU Make
```
make
```

## 打包方式
将samples改名为main，和依赖的动态链接库（如果有）一起打成zip的压缩包,将压缩包上传到子设备驱动
```
cd samples
cp samples main
zip -r abc.zip main
```

## API参考文档

/**
 * @brief 应用初始化
 *
 * @param void: 
 *
 * @retval : 成功则返回APP_OK
 */
 
app_status app_common_init(void)

/**
 * @brief 获取应用名称
 *
 * @param void:               
 *
 * @retval : 成功则返回应用名称的字符串，失败返回NULL
 */
 
char *app_get_name(void)

/**
 * @brief 获取产品SN号的字符串
 *
 * @param void:               
 *
 * @retval : 成功则返回产品SN号的字符串，失败返回NULL
 */
 
char *app_get_productSN(void)

/**
 * @brief 获取设备SN号的字符串
 *
 * @param void:               
 *
 * @retval : 成功则返回设备SN号的字符串，失败返回NULL
 */
 
char *app_get_deviceSN(void)

/**
 * @brief 获取应用配置信息的字符串
 *
 * @param void:               
 *
 * @retval : 成功则返回应用配置信息的字符串，失败返回NULL
 */
 
char *app_get_info(void)

/**
 * @brief 注册下行消息回调函数
 *
 * @param normal_handler: 下行消息回调函数指针           
 *
 * @param rrpc_handler: rrpc消息回调函数指针           
 *
 * @param nats_msg_handle: nats消息回调函数指针           
 *
 * @retval : 成功则返回APP_OK
 */
 
app_status app_register_cb(msg_handler normal_handler, msg_handler rrpc_handler, msg_handler nats_msg_handle)

/**
 * @brief 发送消息，可发送二进制流
 *
 * @param topic: topic名字         
 *
 * @param data: 发送数据内容 
 *
 * @param dataLen: 发送数据长度
 *
 * @retval : 成功则返回APP_OK
 */
app_status app_publish(const char *topic, const char *data, int dataLen)

/**
 * @brief 发送字符串消息
 *
 * @param topic: topic名字         
 *
 * @param data: 发送字符串内容
 *
 * @retval : 成功则返回APP_OK
 */
app_status app_publishString(const char *topic, const char *data)

/**
 * @brief  订阅nats subject
 *
 * @param subject: nats subject名称         
 *
 * @retval : 成功则返回APP_OK
 */
app_status nats_subscribe(const char *subject)

/**
 * @brief 向nats subject发送消息
 *
 * @param subject: nats subject名称
 *
 * @param data: 发送内容
 *
 * @param dataLen: 发送内容长度
 *
 * @retval : 成功则返回APP_OK
 */
app_status nats_publish(const char *subject, const char *data, int dataLen)

/**
 * @brief 记录日志
 *
 * @param level:        日志等级分为：DEBUG、INFO、WARNING、ERROR
 * @param format:       日志记录格式
 *
 * @retval : void
 */
 
void log_write(log_level level, const char *format,...)

