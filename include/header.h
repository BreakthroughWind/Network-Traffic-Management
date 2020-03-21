#include <string.h>
#include <stdbool.h>

#define DATA_LENGTH 20

/* Definition of packets */
typedef struct
{
    char file[DATA_LENGTH];
    bool isLast;
    size_t seq_num;
    size_t length;
} header;

typedef struct
{
    header header;
    char data[DATA_LENGTH];
} packet;

typedef struct
{
    char *flag;
    size_t *count;
} pair;

// sender: build packet function
void build(char *data, size_t slices, packet *packets, pair file);

// receiver: parse packet function
char *parse(packet *pkt);
