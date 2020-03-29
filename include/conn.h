/* MEng Project */

typedef struct
{
    // struct receiver_buffer;
    int receive_size;
    char *receive_buffer;

    int send_size;
    char *send_buffer;

    // tcp connections
    int socket_num;
    int *sockets;

    // mutex: lock and cond
} conn;

// initalization
int conn_init(conn *connection);

// sender: send function
int conn_send(conn *connection, int data_len, char *data);

// receiver: receive function
int conn_read(conn *connection, int data_len, char *data);

// termination
int conn_close(conn *connection);
