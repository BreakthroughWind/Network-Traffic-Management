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

int max_num_pkt;

/* Sort the recevied packets array */
void sort_packets(packet *packet_array)
{
}

/* Reorganize the sorted packets array */
void reorg(packet *packet_array)
{
}

/* Get the number of lines of a file */
int num_line(FILE* fd) 
{
    int lines = 0;
    char c = getc(fd);
    while (c != EOF) 
    {
        if (c == '\n') {
            lines += 1;
        }
        c = getc(fd);
    }
    return lines - 1;
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
    struct connInfo *conn = (struct connInfo *)malloc(sizeof(struct connInfo));
    struct sockaddr_in remoteAddr;

    if ((conn->connFd = accept(listenFd, (struct sockaddr *)&remoteAddr, (socklen_t *)&sin_size)) == -1)
    {
        perror("accept");
        exit(1);
    }
    inet_ntop(AF_INET, &(remoteAddr.sin_addr), conn->remoteIp, INET_ADDRSTRLEN);
    conn->remotePort = (int)ntohs(remoteAddr.sin_port);

#ifdef LOG_INFO
    printf("Server: accept connection from %s:%d\n", conn->remoteIp, conn->remotePort);
#endif
    char file[] = "hello.txt";
    FILE *fptr = fopen(file, "w");
    //read data:
    unsigned char buf[sizeof(packet)];
    while (1)
    {
        int ret = recv(conn->connFd, buf, sizeof(packet), 0);
        if (ret == -1)
        {
            perror("read");
            break;
        }
        if (ret == 0)
        {
            break;
        }
        packet *pkt = malloc(sizeof(packet));
        memcpy(pkt, buf, sizeof(buf));
        printf("data is %s\n", (*pkt).data);
        printf("seq number is %d\n", (*pkt).header.seq_num);
        printf("is last %d\n", (*pkt).header.isLast);
        if (strcmp((*pkt).header.file, file) == 0)
        {
            fprintf(fptr, "%d\n", (*pkt).header.seq_num);
            fprintf(fptr, "%d\n", (*pkt).header.length);
            fprintf(fptr, "%s\n", (*pkt).data);
            
        }
        if ((*pkt).header.isLast == 1)
        {
            max_num_pkt = (*pkt).header.seq_num + 1;
            printf("Total number of packets is %d\n", max_num_pkt);
        }
    }
    fclose(fptr);
    FILE *fd = fopen(file, "r");
    int num = num_line(fd);
    printf("%d lines\n", num);
    fclose(fd);
#ifdef LOG_INFO
    printf("Server: totally close\n");
#endif
    close(listenFd);
}
