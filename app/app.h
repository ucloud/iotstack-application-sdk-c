#ifndef _APP_H
#define _APP_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <time.h>
#include <stdarg.h>
#include <signal.h>
#include <unistd.h>
#include "cJSON.h"
#include "nats.h"
#include "utils.h"

#define APP_MALLOC(s)      malloc((s))
#define APP_CALLOC(c,s)    calloc((c), (s))
#define APP_REALLOC(p, s)  realloc((p), (s))
#define APP_FREE(p)        free((p))

typedef enum
{
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
}log_level;

#define CONFIG_FILE_PATH                    "./etc/iotedge/config.json"    

#define NATS_SERVER_DEFAULT_URL             "tcp://127.0.0.1:4222"

//topic format
#define EDGE_APP_SUBJECT_FORMAT             "edge.app.%s"                 //edge.app.appName
#define EDGE_ROUTER_SUBJECT_FORMAT          "edge.router.%s"              //edge.router.appName

#define EDGE_LOG_UPLOAD_SUBJECT             "edge.log.upload"             //log update
#define EDGE_APP_STATUS_SUBJECT             "edge.app.status"

//payload firmat
#define NORMAL_MSG_FORMAT                   "{\"src\": \"app\",\"topic\": \"%s\",\"payload\": \"%s\",\"identity\": \"%s\"}"
#define STATUS_SYNC_FORMAT                  "{\"appName\": \"%s\"}"
#define LOG_UPLOAD_FORMAT                   "{\"module\": \"application_%s\",\"level\": \"%s\",\"message\": \"%s\",\"timestamp\": %ld}"

#define NATS_MSG_MAX_LEN                    2048
#define NATS_SUBJECT_MAX_LEN                100

typedef enum
{
    APP_OK         = 0,                ///< Success

    APP_ERR,                           ///< Generic error
    APP_PROTOCOL_ERROR,                ///< Error when parsing a protocol message,
                                        ///  or not getting the expected message.
    APP_IO_ERROR,                      ///< IO Error (network communication).
    APP_LINE_TOO_LONG,                 ///< The protocol message read from the socket
                                        ///  does not fit in the read buffer.

    APP_CONNECTION_CLOSED,             ///< Operation on this connection failed because
                                        ///  the connection is closed.
    APP_NO_SERVER,                     ///< Unable to connect, the server could not be
                                        ///  reached or is not running.
    APP_STALE_CONNECTION,              ///< The server closed our connection because it
                                        ///  did not receive PINGs at the expected interval.
    APP_SECURE_CONNECTION_WANTED,      ///< The client is configured to use TLS, but the
                                        ///  server is not.
    APP_SECURE_CONNECTION_REQUIRED,    ///< The server expects a TLS connection.
    APP_CONNECTION_DISCONNECTED,       ///< The connection was disconnected. Depending on
                                        ///  the configuration, the connection may reconnect.

    APP_CONNECTION_AUTH_FAILED,        ///< The connection failed due to authentication error.
    APP_NOT_PERMITTED,                 ///< The action is not permitted.
    APP_NOT_FOUND,                     ///< An action could not complete because something
                                        ///  was not found. So far, this is an internal error.

    APP_ADDRESS_MISSING,               ///< Incorrect URL. For instance no host specified in
                                        ///  the URL.

    APP_INVALID_SUBJECT,               ///< Invalid subject, for instance NULL or empty string.
    APP_INVALID_ARG,                   ///< An invalid argument is passed to a function. For
                                        ///  instance passing NULL to an API that does not
                                        ///  accept this value.
    APP_INVALID_SUBSCRIPTION,          ///< The call to a subscription function fails because
                                        ///  the subscription has previously been closed.
    APP_INVALID_TIMEOUT,               ///< Timeout must be positive numbers.

    APP_ILLEGAL_STATE,                 ///< An unexpected state, for instance calling
                                        ///  #natsSubscription_NextMsg() on an asynchronous
                                        ///  subscriber.

    APP_SLOW_CONSUMER,                 ///< The maximum number of messages waiting to be
                                        ///  delivered has been reached. Messages are dropped.

    APP_MAX_PAYLOAD,                   ///< Attempt to send a payload larger than the maximum
                                        ///  allowed by the NATS Server.
    APP_MAX_DELIVERED_MSGS,            ///< Attempt to receive more messages than allowed, for
                                        ///  instance because of #natsSubscription_AutoUnsubscribe().

    APP_INSUFFICIENT_BUFFER,           ///< A buffer is not large enough to accommodate the data.

    APP_NO_MEMORY,                     ///< An operation could not complete because of insufficient
                                        ///  memory.

    APP_SYS_ERROR,                     ///< Some system function returned an error.

    APP_TIMEOUT,                       ///< An operation timed-out. For instance
                                        ///  #natsSubscription_NextMsg().

    APP_FAILED_TO_INITIALIZE,          ///< The library failed to initialize.
    APP_NOT_INITIALIZED,               ///< The library is not yet initialized.

    APP_SSL_ERROR,                     ///< An SSL error occurred when trying to establish a
                                        ///  connection.

    APP_NO_SERVER_SUPPORT,             ///< The server does not support this action.

    APP_NOT_YET_CONNECTED,             ///< A connection could not be immediately established and
                                        ///  #natsOptions_SetRetryOnFailedConnect() specified
                                        ///  a connected callback. The connect is retried asynchronously.

    APP_DRAINING,                      ///< A connection and/or subscription entered the draining mode.
                                        ///  Some operations will fail when in that mode.

    APP_INVALID_QUEUE_NAME,            ///< An invalid queue name was passed when creating a queue subscription.

} app_status;

typedef void (*msg_handler)(char *topic, char *payload);

char *app_get_name();
char *app_get_productSN();
char *app_get_deviceSN();
char *app_get_info();

app_status app_register_cb(msg_handler normal_handler, msg_handler rrpc_handler);
app_status app_publish(const char *topic, const char *str);
app_status app_common_init(void);
void log_write(log_level level, const char *format,...);
app_status app_rrpc_response(char *topic,char *payload);


#endif
