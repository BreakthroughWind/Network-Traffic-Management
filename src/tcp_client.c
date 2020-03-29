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

/* Serialize the packet */
// unsigned char *serialize(packet *pkt)
// {
//     unsigned char buffer[sizeof(pkt)];
//     memcpy(&buffer, &pkt, sizeof(pkt));
//     return &buffer;
// }

// packet *deserialize(unsigned char *data)
// {
//     packet *pkt;

//     pkt = (packet *)data;
//     return pkt;
// }

void int_to_char(int n, char *ptr) 
{   
    unsigned char bytes[4];

    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    printf("result of int to char is ");
    for (int i = 0; i < 4; ++i) {
        *ptr = bytes[i];
        printf("%c", ptr);
        ptr++;
    }
}

void char_to_int(char *cptr, int *nptr)
{
    *nptr = (cptr[0] << 24) + (cptr[1] << 16) + (cptr[2] << 8) + (cptr[3] << 0);
    cptr = cptr + 4;
}

void serialize(packet* pkt, char *data)
{
    char *ptr = (char*)data;    
    for (int i = 0; i < DATA_LENGTH; ++i) {
      if (i < strlen(pkt->header.file)) {
        *ptr = pkt->header.file[i];
      }
      else {
        *ptr = 127;
      }
      ++ptr;
    }
    int_to_char(pkt->header.isLast, ptr);
    int_to_char(pkt->header.seq_num, ptr);
    int_to_char(pkt->header.length, ptr);
    
    for (int i  = 0; i < strlen(pkt->data); ++i) {
      if (*ptr != 127) {
        *ptr = pkt->data[i];
      }
      
      ++ptr;
    }
    
}
void deserialize(char *data, packet* pkt)
{
    printf("First line of deserialize\n");
    char *ptr = (char*)data;    
    for (int i = 0; i < DATA_LENGTH; ++i) {
        if (*ptr != 127) {   
            pkt->header.file[i] = *ptr;
        }
        ++ptr;
    }
    char_to_int(ptr, &(pkt->header.isLast));
    char_to_int(ptr, &(pkt->header.seq_num));
    char_to_int(ptr, &(pkt->header.length));
    printf("file name is %s\n", pkt->header.file);
    printf("is last %d\n", pkt->header.isLast);
    printf("sequence number %d\n", pkt->header.seq_num);
    printf("length is %d\n", pkt->header.length);
    strncpy(&(pkt->data), ptr, pkt->header.length);
    // printf("data is %s", pkt->data);
}

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
        printf("temp length is %ld, buff is %s\n", strlen(temp), temp);
        
        int slices = (int)ceil((double)strlen(temp) / DATA_LENGTH);
        printf("number of slices is %d\n", slices);
        pktarr = split_data(temp, &file, slices);

        for (int i = 0; i < slices; ++i)
        {
            char *ser_buff = malloc(sizeof(packet));
            printf("Before serialize\n");
            serialize(pktarr++, ser_buff);
            send(connFd, ser_buff, sizeof(packet), 0);
            packet *pkt;
            pkt = malloc(sizeof(int) * 3 + DATA_LENGTH * 2 * sizeof(char));
            printf("Before deserialize\n");
            deserialize(ser_buff, pkt);
            free(ser_buff);
            free(pkt);
        }   
    }
    fclose(fd);
    return 0;
}