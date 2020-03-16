
// include

/* Definition of packets */
struct header {
    int seq_num;
    int length;
}

struct packet {
    struct header;
    char* data;
    int flag;
}

// sender: build packet function
packet* build(char* data, int count, int isLast) {
    int len = sizeof(&data);
    // Create header for this piece of data
    header* h;
    h->seq_num = count;
    h->length = len;
    packet* pkt;
    pkt->header = h;
    pkt->data = data;
    pkt->flag = isLast;
    return pkt;
}
// receiver: parse packet function
char* parse(packet* pkt) {
    int len = sizeof(pkt.data);
    char parsed_data[len];
    for (int i = 0; i < len; ++i) {
        prased_data[i] = pkt->data[i];
    } 
    if (pkt->signal == 2) {
        // signal to do reorg
    }
    return parsed_data;
}
