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
packet *split_data(char *raw_data, pair file)
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
void build(char *data, size_t slices, packet *packets, pair file)
{
    size_t data_len = strlen(data);

    if (slices == 1)
    {
        // update packet -> header -> flag
        strcpy(packets[0].header.file, file.flag);
        // update packet -> header -> seq_num
        packets[0].header.seq_num = *file.count;
        (*file.count) += 1;
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
            strcpy(packets[i].header.file, file.flag);
            // update packet -> header -> seq_num
            packets[i].header.seq_num = *file.count;
            (*file.count) += 1;
            // update packet -> header -> length
            packets[i].header.length = (i != slices - 1) ? DATA_LENGTH : data_len % DATA_LENGTH;

            // update packet -> data
            strncpy(packets[i].data, data + DATA_LENGTH * i, packets[i].header.length);
        }
    }
}

int main(int argc, char *argv[])
{
    sender();
}

//  Use passive probing to test the condition of the path
// double probe(pathInfo* path_array);

// implement sender function
int sender()
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
    // remote_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // Do the conversion
    inet_ntop(AF_INET, &(remote_addr.sin_addr), remote_ip, INET_ADDRSTRLEN);
    remote_port = (int)ntohs(remote_addr.sin_port);

    // Connect the socket to the remote server
    if (connect(conn_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr) == -1))
    {
        perror("connect failed");
        exit(1);
    }
    // printf("connection to %s established", argv[1]);

    char buff[packet_length];
    // We don't need to close the connection manually.
    FILE *fd = fopen("../hello.txt", "r");
    pair pair = {"hello.txt", 0};
    while (fgets(buff, packet_length, fd) != NULL)
    {
        char *temp = malloc(strlen(buff) * sizeof(char));
        strcpy(temp, buff);
        packet *pktarr = split_data(temp, pair);
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

int receiver(int argc, int *argv[])
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
}
