#include <stdio.h>
#include <unistd.h>

// int main(void) {
//         sleep(2);
//         printf("Hello, world!\n");
//         return 0;
// }

/* 
 * daytime.c: a simple network service based on lwIP and mini-os
 * 
 * Tim Deegan <Tim.Deegan@eu.citrix.net>, July 2007
 */

#include <os.h>
#include <xmalloc.h>
#include <console.h>
#include <netfront.h>
#include <lwip/api.h>

static char message[29];

void run_daytime(void *p)
{
    struct ip_addr listenaddr = { 0 };
    struct netconn *listener;
    struct netconn *session;
    struct timeval tv;
    u16_t listen_port=49152; //13;

    err_t rc;

    //start_networking();

    if (0) {
        struct ip_addr ipaddr = { htonl(0x0a000001) };
        struct ip_addr netmask = { htonl(0xffffff00) };
        struct ip_addr gw = { 0 };
        networking_set_addr(&ipaddr, &netmask, &gw);
    }

    printf("Opening connection\n");

    listener = netconn_new(NETCONN_TCP);
    printf("Connection at %p, port: %d\n", listener, listen_port);

    //rc = netconn_bind(listener, &listenaddr, listen_port);
    rc = netconn_bind(listener, IP_ADDR_ANY, listen_port);
    if (rc != ERR_OK) {
        printf("Failed to bind connection: %i\n", rc);
        return;
    }
    printf("success bind port: %u\n", listen_port);

    rc = netconn_listen(listener);
    if (rc != ERR_OK) {
        printf("Failed to listen on connection: %i\n", rc);
        return;
    }

    printf("now listening on connection\n");

    while (1) {

        session = netconn_accept(listener);
        if (session == NULL) 
            continue;
        //rc = netconn_accept(listener, &session);
        //if (rc != ERR_OK)
        //   continue;

        gettimeofday(&tv, NULL);
        sprintf(message, "%20lu.%6.6lu\n", tv.tv_sec, tv.tv_usec);
        (void) netconn_write(session, message, strlen(message), NETCONN_COPY);
        (void) netconn_disconnect(session);
        (void) netconn_delete(session);
    }
}


int start_daytime(void)
{
    int i ;

    create_thread("daytime", run_daytime, NULL);

    for (i = 0; i < 10000; i++){
        sleep(200000);
    }
    return 0;
}
