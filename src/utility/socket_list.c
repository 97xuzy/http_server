#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "socket_list.h"


int create_socket_list(socket_conn_list_t *list, int max_size)
{
    list->ptr = calloc(max_size, sizeof(int));
    list->size = 0;
    list->alloc_size = max_size * sizeof(int);
    list->max_size = max_size;
    return 0;
}

int add_socket_to_list(socket_conn_list_t *list, int socket)
{
    if(list->size + 1 > list->max_size)
    {
        return -1;
    }
    list->ptr[list->size] = socket;
    list->size++;

    return 0;
}

int remove_socket_to_list(socket_conn_list_t *list, int socket)
{
    for(size_t i = 0; i < 0; i++)
    {
        if(list->ptr[i] == socket)
        {
            list->ptr[i] = list->ptr[list->size-1];
            list->ptr[list->size - 1] = 0;
            list->size--;
            return 0;
        }
    }
    // If not found
    return -1;
}

size_t size_of_socket_list(socket_conn_list_t *list)
{
    return list->size;
}

int destroy_socket_list(socket_conn_list_t *list)
{
    free(list->ptr);
    return 0;
}



