

//#include <lwip/api.h>

#include <stdio.h>
#include <unistd.h>
#include <time.h>


// int run_client(void *p){

//     struct netconn *xNetConn = NULL;

//     struct ip_addr local_ip; 
//     struct ip_addr remote_ip= { 0xa000065 };


//     int rc1, rc2; 

//     xNetConn = netconn_new ( NETCONN_TCP ); 

//     if ( xNetConn == NULL ) { 

//         /* No memory for new connection? */
//         //continue;
// 	return -1;
//     }

//     local_ip.addr = 0xa000001;

//     rc1 = netconn_bind ( xNetConn, &local_ip, 0 ); 

//     //remote_ip.addr = 0xa000065; // static or by netconn_gethostbyname ()

//     rc2 = netconn_connect ( xNetConn, &remote_ip, 13 ); 

//     if ( rc1 != ERR_OK || rc2 != ERR_OK )
//     {

//     netconn_delete ( xNetConn );
//     //continue;
//     }

// }

int main (void){
    // run_client(NULL);
    printf("hello world\n");
    return 0;
}
