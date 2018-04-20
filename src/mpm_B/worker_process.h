#ifndef WORKER_PROCESS_H
#define WORKER_PROCESS_H

#include "../utility/flag.h"

typedef struct conn_thread
{
    pthread_t thread;
    flag_t exit_flag;
    int serv_sock;
    int conn_epoll;
} conn_thread_t;

typedef struct req_thread
{
    pthread_t thread;
    int conn_epoll;
    struct epoll_event *event_array;
    flag_t exit_flag;
} req_thread_t;


/********************
 * Worker Process
********************/
int worker_process(int serv_sock, int pipe_fd[2]);




#endif

