/* MEng Project */

struct conn {
    // struct receiver_buffer;
    int receive_size;
    char* receive_buffer;

    int send_size;
    char* send_buffer;

    // tcp connections
    int socket_num;
    int* sockets;

    // mutex: lock and cond
};

// initalization
int conn_init(struct conn) {

}

// sender: send function
int conn_send(struct conn, int data_len, int* data){

}

// receiver: receive function
int conn_read(struct conn, int data_len, int* data){
    
}

// termination
int conn_close(struct conn){
    
}

