#include <string.h>

/* Definition of packets */
typedef struct
{
    char *flag;
    size_t seq_num;
    size_t length;
} header;

typedef struct
{
    header header;
    char *data;
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
