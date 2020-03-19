
/* Definition of packets */
typedef struct  {
    int seq_num;
    int length;
} header;
typedef struct {
    struct header;
    char* data;
    int flag;
} packet;

// sender: build packet function
packet *build(char* data, int count, int isLast) {
    
}
// receiver: parse packet function
char* parse(packet* pkt) {

}
