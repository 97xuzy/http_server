#ifndef WORKER_PROCESS_B_H
#define WORKER_PROCESS_B_H

#include "../utility/flag.h"
#include "../config.h"

typedef struct conn_thread
{
    pthread_t thread;
    flag_t exit_flag;
    int serv_sock;
    int conn_epoll;
    config_t *local_config;
} conn_thread_t;

typedef struct req_thread
{
    pthread_t thread;
    int conn_epoll;
    struct epoll_event *event_array;
    flag_t exit_flag;
    config_t *local_config;
} req_thread_t;


/********************
 * Worker Process
********************/
int worker_process_B(int serv_sock, int pipe_fd[2], config_t *local_config);




#endif

