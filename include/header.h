#include <string.h>
#include <stdbool.h>

#define DATA_LENGTH 20

/* Definition of packets */
typedef struct
{
    char file[DATA_LENGTH];
    bool isLast;
    int seq_num;
    int length;
} header;

typedef struct
{
    header header;
    char data[DATA_LENGTH];
} packet;

typedef struct
{
    char file_name[DATA_LENGTH];
    int count;
} pair;

// sender: build packet function
void build(char *data, int slices, packet *packets, pair *file);

// receiver: parse packet function
char *parse(packet *pkt);
