#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/header.h"
#include "../include/conn.h"

#define PORT 8080
#define BACKLOG 128
#define DATA_LENGTH 20
#define LOG_INFO 1
// #define PACKET_LENGTH 1500

/* Sort the recevied packets array */
void sort_packets(packet *packet_array)
{
}

/* Reorganize the sorted packets array */
void reorg(packet *packet_array)
{
}

/* Parse the received data */
char *parse(packet *pkt)
{
    char *data;
    data = malloc(strlen(pkt->data));
    strcpy(data, pkt->data);
    return data;
}

struct connInfo
{
    int connFd;
    char remoteIp[INET_ADDRSTRLEN];
    int remotePort;
};

// implement receiver function

int main(int argc, char *argv[])
{
    //connection info:
    int sin_size = sizeof(struct sockaddr_in);

    int listenFd;
    struct sockaddr_in listenAddr;
    char listenIp[INET_ADDRSTRLEN];
    int listenPort;

    if ((listenFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    //initialization:
    bzero(&listenAddr, sizeof(struct sockaddr_in));
    listenAddr.sin_family = AF_INET;
    listenAddr.sin_port = htons(PORT);
    listenAddr.sin_addr.s_addr = inet_addr(argv[1]);

    inet_ntop(AF_INET, &(listenAddr.sin_addr), listenIp, INET_ADDRSTRLEN);
    listenPort = (int)ntohs(listenAddr.sin_port);

    if (bind(listenFd, (struct sockaddr *)&listenAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("bind error");
        exit(1);
    }

    if (listen(listenFd, BACKLOG) == -1)
    {
        perror("listen error");
        exit(1);
    }

    //successfully set up TCP connection:
    printf("Server: listen on %s:%d\n", listenIp, listenPort);
    
   //Create a socket to communicate with the client
   struct connInfo* conn = (struct connInfo*) malloc(sizeof(struct connInfo));
   struct sockaddr_in remoteAddr;

   if ((conn->connFd = accept(listenFd, (struct sockaddr*)&remoteAddr, (socklen_t*) &sin_size)) == -1)
   {
       perror("accept");
       exit(1);
   }
   inet_ntop(AF_INET, &(remoteAddr.sin_addr), conn->remoteIp, INET_ADDRSTRLEN);
   conn->remotePort = (int) ntohs(remoteAddr.sin_port);

#ifdef LOG_INFO
   printf("Server: accept connection from %s:%d\n", conn->remoteIp, conn->remotePort);
#endif

    //read data:
    char buf[sizeof(packet)];
    printf("size of packet is %d\n", sizeof(packet));
    while(1)
        {
            int ret = read(conn->connFd, buf, sizeof(packet));
            if(ret == -1)
            {
                perror("read");
                break;
            }
            if(ret == 0)
            {
                break;
            }
            printf("size of buf %d and content is %s\n", strlen(buf), buf);
            // write(conn->connFd, buf, ret);
 }
#ifdef LOG_INFO
    printf("Server: totally close\n");
#endif
    close(listenFd);
}
