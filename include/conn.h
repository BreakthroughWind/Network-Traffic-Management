/* MEng Project */

typedef struct
{
    // struct receiver_buffer;
    size_t receive_size;
    char *receive_buffer;

    size_t send_size;
    char *send_buffer;

    // tcp connections
    size_t socket_num;
    size_t *sockets;

    // mutex: lock and cond
} conn;

// initalization
size_t conn_init(conn *connection);

// sender: send function
size_t conn_send(conn *connection, size_t data_len, char *data, pair file);

// receiver: receive function
size_t conn_read(conn *connection, size_t data_len, char *data);

// termination
size_t conn_close(conn *connection);
