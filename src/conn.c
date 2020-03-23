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
packet *split_data(char *raw_data)
{
}

/* Reorganize the sorted packets array */
void reorg(packet *packet_array)
{
}

/* Build the packet */
packet build(char *data, int count, int isLast)
{
}

//  Use passive probing to test the condition of the path
// double probe(pathInfo* path_array);

// implement sender function
int sender(int argc, char *argv[])
{
    int packet_length = sizeof(packet);
    int conn_fd;
    // first, build the connection
    struct sockaddr_in remote_addr;
    // Declare the array for holding the IP, INET_ADDRSERLEN is the length of
    char remote_ip[INET_ADDRSTRLEN];
    int remote_port;
    char buff[packet_length];

    // AF_INET stands for IPv4 protocol, SOCK_STREAM stands for TCP, 0 stands for 0.
    if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    // Initialize the remote_addr specification
    bzero(&remote_addr, sizeof(struct sockaddr_in));

    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(PORT);
    // inet_addr convert the standard IPv4 dotted notation to integer value format
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Do the conversion
    inet_ntop(AF_INET, &(remote_addr.sin_addr), remote_ip, INET_ADDRSTRLEN);
    remote_port = (int)ntohs(remote_addr.sin_port);

    // Connect the socket to the remote server
    if (connect(conn_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr) == -1))
    {
        perror("connect failed");
        exit(1);
    }
    printf("connection to %s established", argv[1]);

    // We don't need to close the connection manually.
    FILE *fd = fopen("../hello.txt", "r");
    while (fgets(buff, packet_length, fd) != NULL)
    {
        char *temp = malloc(strlen(buff) * sizeof(char));
        strcpy(temp, buff);
        packet *pktarr = split_data(temp);
        for (int i = 0; (pktarr + i) != NULL; ++i)
        {
            // need to figure out what size this should be 
            send(conn_fd, pktarr + i, sizeof(packet) + DATA_LENGTH, 0);
        }
        // check the content of the buff
        printf("content is %s", buff);
    }
    fclose(fd);
}

struct connInfo
{
    int connFd;
    char remoteIp[INET_ADDRSTRLEN];
    int remotePort;
};

// implement receiver function

int receiver(int argc, char *argv[])
{
    //connection info:
    int sin_size = sizeof(struct sockaddr_in);

    int listenFd;
    struct sockaddr_in listenAddr;
    char listenIp[INET_ADDRSTRLEN];
    int listenPort;

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

int main() {
    int argc = 2;
    char *argv[2];
    argv[0] = "conn.out";
    argv[1] = "127.0.0.1";
    printf("going to start");
    receiver(argc, argv); 
    sender(argc, argv);
    return 0;
}