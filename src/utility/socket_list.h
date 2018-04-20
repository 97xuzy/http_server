#ifndef SOCKET_LIST_H
#define SOCKET_LIST_H


typedef struct socket_conn_list
{
    int *ptr;
    size_t alloc_size;
    size_t size;
    size_t max_size;
} socket_conn_list_t;

int create_socket_list(socket_conn_list_t *list, int max_size);
int add_socket_to_list(socket_conn_list_t *list, int socket);
int remove_socket_to_list(socket_conn_list_t *list, int socket);
size_t size_of_socket_list(socket_conn_list_t *list);
int destroy_socket_list(socket_conn_list_t *list);




#endif

