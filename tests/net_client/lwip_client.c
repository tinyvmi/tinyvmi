

#include <lwip/api.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>


// #define SENDER_PORT_NUM 6000 
// #define SENDER_IP_ADDR "10.0.0.100"

// #define SERVER_PORT_NUM 6001 
// #define SERVER_IP_ADDRESS "10.0.0.100"


// void run_server(void *p)
// {

//     int socket_fd,accept_fd;
//     int addr_size,sent_data; 
//     char data_buffer[80]; 
//     struct sockaddr_in sa,ra,isa;

//     /* Creates an TCP socket (SOCK_STREAM) with Internet Protocol Family (PF_INET).
//     * Protocol family and Address family related. For example PF_INET Protocol Family and AF_INET family are coupled.
//     */

//     socket_fd = socket(PF_INET, SOCK_STREAM, 0);

//     if ( socket_fd < 0 )
//     {

//         printf("socket call failed");
//         exit(0);
//     }

//     memset(&sa, 0, sizeof(struct sockaddr_in));
//     sa.sin_family = AF_INET;
//     sa.sin_addr.s_addr = inet_addr(SENDER_IP_ADDR);
//     sa.sin_port = htons(SENDER_PORT_NUM);


//     /* Bind the TCP socket to the port SENDER_PORT_NUM and to the current
//     * machines IP address (Its defined by SENDER_IP_ADDR). 
//     * Once bind is successful for UDP sockets application can operate
//     * on the socket descriptor for sending or receiving data.
//     */
//     if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(sa)) == -1)
//     {
//         printf("Bind to Port Number %d ,IP address %s failed\n",             
//                 SENDER_PORT_NUM,SENDER_IP_ADDR);
//         close(socket_fd);
//         exit(1);
//     }

//     listen(socket_fd,5);
//     addr_size = sizeof(isa);
//     accept_fd = accept(socket_fd, (struct sockaddr*)&isa,&addr_size);
//     if(accept_fd < 0)
//     {
//         printf("accept failed\n");
//         close(socket_fd);
//         exit(2);
//     }
   
//     strcpy(data_buffer,"Hello World\n");
//     sent_data = send(accept_fd, data_buffer,sizeof("Hello World",0));

//     if(sent_data < 0 )
//     {

//         printf("send failed\n");
//         close(socket_fd);
//         exit(3);
//     }

//     close(socket_fd);
// }




// void run_client(void* p)
// {

//     int socket_fd;
//     struct sockaddr_in sa,ra;

//     int recv_data; char data_buffer[80]; /* Creates an TCP socket (SOCK_STREAM) with Internet Protocol Family (PF_INET).
//     * Protocol family and Address family related. For example PF_INET Protocol Family and AF_INET family are coupled.
//     */

//     socket_fd = socket(PF_INET, SOCK_STREAM, 0);

//     if ( socket_fd < 0 )
//     {
//         printf("socket call failed");
//         exit(0);
//     }

//     memset(&sa, 0, sizeof(struct sockaddr_in));
//     sa.sin_family = AF_INET;
//     sa.sin_addr.s_addr = inet_addr(SENDER_IP_ADDR);
//     sa.sin_port = htons(SENDER_PORT_NUM);


//     /* Bind the TCP socket to the port SENDER_PORT_NUM and to the current
//     * machines IP address (Its defined by SENDER_IP_ADDR). 
//     * Once bind is successful for UDP sockets application can operate
//     * on the socket descriptor for sending or receiving data.
//     */
//     if (bind(socket_fd, (struct sockaddr *)&sa, sizeof(struct sockaddr_in)) == -1)
//     {
//         printf("Bind to Port Number %d ,IP address %s failed\n",SENDER_PORT_NUM,SENDER_IP_ADDR);
//         close(socket_fd);
//         exit(1);
//     }
//     /* Receiver connects to server ip-address. */

//     memset(&ra, 0, sizeof(struct sockaddr_in));
//     ra.sin_family = AF_INET;
//     ra.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);
//     ra.sin_port = htons(SERVER_PORT_NUM);


//     if(connect(socket_fd,(struct sockaddr_in*)&ra,sizeof(struct sockaddr_in)) < 0)
//     {
//         printf("connect failed \n");
//         close(socket_fd);
//         exit(2);
//     }

//     recv_data = recv(socket_fd,data_buffer,sizeof(data_buffer),0);

//     if(recv_data < 0)
//     {
//         printf("recv failed \n");
//         close(socket_fd);
//         exit(2);
//     }
//     data_buffer[recv_data] = '\0';
//     printf("received data: %s\n",data_buffer);

//     close(socket_fd);
// }



// int start_server(void *p)
// {
//     // create_thread("server", run_server, p);
//     return 0;
// }


// int start_client(void *p){

//     // create_thread("client", run_client, p);
//     return 0;
// }

int run_client(void *p){

    struct netconn *xNetConn = NULL;

    struct ip_addr local_ip; 
    struct ip_addr remote_ip= { 0xa000065 };


    int rc1, rc2; 

    xNetConn = netconn_new ( NETCONN_TCP ); 

    if ( xNetConn == NULL ) { 

        /* No memory for new connection? */
        //continue;
	return -1;
    }

    local_ip.addr = 0xa000001;

    rc1 = netconn_bind ( xNetConn, &local_ip, 0 ); 

    //remote_ip.addr = 0xa000065; // static or by netconn_gethostbyname ()

    rc2 = netconn_connect ( xNetConn, &remote_ip, 13 ); 

    if ( rc1 != ERR_OK || rc2 != ERR_OK )
    {

    netconn_delete ( xNetConn );
    //continue;
    }

}

int main (void){
    run_client(NULL);
    return 0;
}
