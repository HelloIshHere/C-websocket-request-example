#include <stdio.h>
#include <string.h>
#include <libwebsockets.h>

#define EXAMPLE_HOST "example.com"
#define EXAMPLE_URI "/websocket"

static int callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    switch (reason) {
        case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
            fprintf(stderr, "Connection error: %s\n", (char *) in);
            break;
        case LWS_CALLBACK_CLIENT_RECEIVE:
            printf("Received data: %s\n", (char *) in);
            break;
        case LWS_CALLBACK_CLIENT_WRITEABLE:
            printf("Sending data...\n");
            lws_write(wsi, (unsigned char *) "Hello, server!", 14, LWS_WRITE_TEXT);
            break;
        default:
            break;
    }
    return 0;
}

int main(int argc, char **argv) {
    struct lws_context_creation_info info;
    struct lws_client_connect_info connect_info;
    struct lws_context *context;
    struct lws *wsi;
    int ret;

    memset(&info, 0, sizeof(info));
    info.port = CONTEXT_PORT_NO_LISTEN;
    info.protocols = lws_protocols;
    info.gid = -1;
    info.uid = -1;

    context = lws_create_context(&info);
    if (!context) {
        fprintf(stderr, "Failed to create context\n");
        return 1;
    }

    memset(&connect_info, 0, sizeof(connect_info));
    connect_info.context = context;
    connect_info.address = EXAMPLE_HOST;
    connect_info.path = EXAMPLE_URI;
    connect_info.port = 443;
    connect_info.ssl_connection = LCCSCF_USE_SSL;

    wsi = lws_client_connect_via_info(&connect_info);
    if (!wsi) {
        fprintf(stderr, "Failed to connect\n");
        lws_context_destroy(context);
        return 1;
    }

    printf("Connected to %s%s\n", EXAMPLE_HOST, EXAMPLE_URI);

    while (1) {
        ret = lws_service(context, 0);
        if (ret < 0) {
            break;
        }
    }

    lws_context_destroy(context);

    return 0;
}
