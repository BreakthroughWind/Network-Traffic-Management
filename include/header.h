#include <string.h>
#include <stdbool.h>

#define DATA_LENGTH 20

/* Definition of packets */
typedef struct
{
    char *file;
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
packet build(char *data, size_t count, size_t isLast);

// receiver: parse packet function
char *parse(packet *pkt);
