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

// #define PACKET_LENGTH 1500

/* Sort the recevied packets array */
void sort_packets(packet *packet_array)
{
}

/* Split the raw data input to multiple slices */
packet *split_data(char *raw_data, pair *file)
{
    // calculate the packet number given raw_data
    size_t slices = (size_t)ceil((double)strlen(raw_data) / DATA_LENGTH);
    packet *packets = malloc(slices * sizeof(packet));
    // build each slice data into packets
    build(raw_data, slices, packets, file);
    return packets;
}

/* Reorganize the sorted packets array */
void reorg(packet *packet_array)
{
}

/* Build the packet */
void build(char *data, size_t slices, packet *packets, pair *file)
{
    size_t data_len = strlen(data);

    if (slices == 1)
    {
        // update packet -> header -> flag
        strcpy(packets[0].header.file, file->flag);
        // update packet -> header -> seq_num
        packets[0].header.seq_num = file->count;
        file->count += 1;
        // update packet -> header -> length
        packets[0].header.length = data_len;

        // update packet -> data
        strncpy(packets[0].data, data + 0 * DATA_LENGTH, packets[0].header.length);
    }
    else
    {
        for (size_t i = 0; i < slices; i++)
        {
            // update packet -> header -> flag
            strcpy(packets[i].header.file, file->flag);
            // update packet -> header -> seq_num
            packets[i].header.seq_num = file->count;
            file->count += 1;
            // update packet -> header -> length
            packets[i].header.length = (i != slices - 1) ? DATA_LENGTH : data_len % DATA_LENGTH;

            // update packet -> data
            strncpy(packets[i].data, data + DATA_LENGTH * i, packets[i].header.length);
        }
    }
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
  char buf[20];
  while(1)
        {
            int ret = read(conn->connFd, buf, 19);
            if(ret == -1)
            {
                perror("read");
                break;
            }
            if(ret == 0)
            {
                break;
            }
            printf("%s\n", buf);
            write(conn->connFd, buf, ret);
 }
#ifdef LOG_INFO
    printf("Server: totally close\n");
#endif
    close(listenFd);
}
