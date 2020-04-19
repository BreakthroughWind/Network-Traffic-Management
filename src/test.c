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
#include "../include/map.h"
#include "../include/vector.h"

#define PORT 8080
#define BACKLOG 128
#define DATA_LENGTH 20
#define LOG_INFO 1
// #define PACKET_LENGTH 1500
#define LINE 1024

int num_line(FILE *fd)
{
    int lines = 0;
    char c = getc(fd);
    while (c != EOF)
    {
        if (c == '\n')
        {
            lines += 1;
        }
        c = getc(fd);
    }
    return lines - 1;
}

int main(int argc, char *argv[])
{
    char *unsorted_file = "hello.txt";
    char *sorted_file = "sorted_file";

    FILE *unsorted_fd = fopen(unsorted_file, "r");
    FILE *sorted_fd = fopen(sorted_file, "w");

    int nums = num_line(unsorted_fd);
    rewind(unsorted_fd);

    struct DSHashMap *hash;
    hash = ds_hashmap_create();

    for (int i = 0; i < nums; i += 3)
    {
        char *buf_seq = (char *)malloc(LINE * sizeof(char));
        char *buf_len = (char *)malloc(LINE * sizeof(char));
        char *buf_data = (char *)malloc(LINE * sizeof(char));

        buf_seq = fgets(buf_seq, LINE, unsorted_fd);
        buf_len = fgets(buf_len, LINE, unsorted_fd);
        buf_data = fgets(buf_data, LINE, unsorted_fd);

        ds_puti(hash, atoi(buf_seq), buf_data);
    }

    for (int i = 0; i < nums / 3; i++)
    {
        fprintf(sorted_fd, "%s", (char *)ds_geti(hash, (int32_t)i));
        printf("%s", (char *)ds_geti(hash, (int32_t)i));
    }

    ds_hashmap_free(hash, false, false);

    fclose(unsorted_fd);
    fclose(sorted_fd);
}