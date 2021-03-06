#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/header.h"
#include "../include/conn.h"

#define PORT 8080
#define BACKLOG 128
#define DATA_LENGTH 20

/* Build the packet */
void build(char *data, int slices, packet *packets, pair *file)
{
    int data_len = strlen(data);
    if (slices == 1)
    {
        // update packet -> header -> flag
        strcpy(packets[0].header.file, file->file_name);
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
        for (int i = 0; i < slices; i++)
        {
            // update packet -> header -> flag
            strcpy(packets[i].header.file, file->file_name);
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

/* Split the raw data input to multiple slices */
packet *split_data(char *raw_data, pair *file, int slices)
{
    packet *packets = malloc(slices * sizeof(packet));
    // build each slice data into packets
    build(raw_data, slices, packets, file);
    return packets;
}

int main(int argc, char *argv[])
{
    int connFd;
    struct sockaddr_in remoteAddr;
    char remoteIp[INET_ADDRSTRLEN];
    int remotePort;

    if (argc != 2)
    {
        printf(stderr, "usage: client hostname\n");
        exit(1);
    }

    if ((connFd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&remoteAddr, sizeof(struct sockaddr_in));
    remoteAddr.sin_family = AF_INET;
    remoteAddr.sin_port = htons(PORT);
    remoteAddr.sin_addr.s_addr = inet_addr(argv[1]);

    inet_ntop(AF_INET, &(remoteAddr.sin_addr), remoteIp, INET_ADDRSTRLEN);
    remotePort = (int)ntohs(remoteAddr.sin_port);

    if (connect(connFd, (struct sockaddr *)&remoteAddr, sizeof(struct sockaddr)) == -1)
    {
        perror("connect");
        exit(1);
    }

    int packet_length = sizeof(packet);
    char buff[packet_length];

    FILE *fd = fopen("../hello.txt", "r");
    pair file = {"hello.txt", 0};

    packet *pktarr = NULL;

    while (fgets(buff, packet_length, fd) != NULL)
    {
        char *temp = malloc(strlen(buff));
        strcpy(temp, buff);

        int slices = (int)ceil((double)strlen(temp) / DATA_LENGTH);
        pktarr = split_data(temp, &file, slices);

        for (int i = 0; i < slices; ++i)
        {
            char *ser_buff = malloc(sizeof(packet));
            memcpy(ser_buff, pktarr++, sizeof(packet));
            send(connFd, ser_buff, sizeof(packet), 0);
            free(ser_buff);
        }
    }
    fclose(fd);
    return 0;
}
