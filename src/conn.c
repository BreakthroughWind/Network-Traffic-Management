#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "../include/header.h"
#include "../include/conn.h"

#define PORT 8080

/* Sort the recevied packets array */
void sort_packets(packet* packet_array){

}

/* Split the raw data input to multiple slices */
packet* split_data(char* raw_data) {
    return NULL;
}


/* Reorganize the sorted packets array */
void reorg(packet* packet_array){
    
}


//  Use passive probing to test the condition of the path 
// double probe(pathInfo* path_array);

// implement sender function
int sender(int argc, char* argv[]) {
    int packet_length = sizeof(packet);
    // int conn_fd;
    // // first, build the connection
    // struct sockaddr_in remote_addr;
    // // Declare the array for holding the IP, INET_ADDRSERLEN is the length of 
    // char remote_ip[INET_ADDRSTRLEN];
    // int remote_port;

    // // AF_INET stands for IPv4 protocol, SOCK_STREAM stands for TCP, 0 stands for 0.
    // if ((conn_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    // {
    //     perror("socket");
    //     exit(1);
    // }

    // // Initialize the remote_addr specification
    // bzero(&remote_addr, sizeof(struct sockaddr_in));
    // remote_addr.sin_family = AF_INET;
    // remote_addr.sin_port = htons(PORT);
    // // inet_addr convert the standard IPv4 dotted notation to integer value format
    // remote_addr.sin_addr.s_addr = inet_addr(argv[1]);

    // // Do the conversion
    // inet_ntop(AF_INET, &(remote_addr.sin_addr), remote_ip, INET_ADDRSTRLEN);
    // remote_port = (int) ntohs(remote_addr.sin_port);

    // // Connect the socket to the remote server
    // if (connect(conn_fd, (struct sockaddr*)&remote_addr, sizeof(struct sockaddr) == -1)) {
    //     perror("connect failed");
    //     exit(1);
    // }
    // printf("connection to %s established", argv[1]);
// 
    char buff[packet_length];
    // We don't need to close the connection manually.
    FILE *fd = fopen("../hello.txt", "r");
    while (fgets(buff, packet_length, fd) != NULL) {
        char *temp = malloc(strlen(buff) * sizeof(char));
        strcpy(temp, buff);
        packet *pktarr = split_data(temp);
        for (int i = 0; (pktarr + i) != NULL; ++i) {
            // need to figure out what
            write(conn_fd, pktarr + i, sizeof(header));
        }
    } 
    fclose(fd);
}

// implement receiver function


int receiver(int argc, int* argv[]) {
    return 0;
}

int main() {
    char *argv[2];
    argv[0] = "name of the file";
    argv[1] = "127.0.0.1";
    sender(2, argv);
    return 0;
}

