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
#include <./include/header.h>
#include <./include/conn.h>

#define PORT 8080;

/* Sort the recevied packets array */
void sort_packets(packet* packet_array){

}

/* Split the raw data input to multiple slices */
void split_data(char* raw_data) {

}


/* Reorganize the sorted packets array */
void reorg(packet* packet_array){

}

//  Use passive probing to test the condition of the path 
// double probe(pathInfo* path_array);

// implement sender function
int sender(int argc, int* argv[]) {
    int conn_fd;
    // first, build the connection
    struct sockaddr_in remote_addr;
    // Declare the array for holding the IP, INET_ADDRSERLEN is the length of 
    char remote_ip[INET_ADDRSTRLEN];
    int remote_port;

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
    remote_addr.sin_addr.s_addr = inet_addr(argv[1]);

    while (1) {

    }
}

// implement receiver function
int receiver(int argc, int* argv[]) {

}


