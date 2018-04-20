#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>

#include "worker_process.h"

#include "handle_request.h"

#define SOCKET_NUM 3000
#define MAX_EVENT_NUM 100



int listen_connection(int serv_sock);

int init_conn_thread(conn_thread_t *thread, int serv_sock);
int spawn_conn_thread(conn_thread_t *thread);
void *connection_thread_func(void *conn_thrd_init_data_ptr);
int close_conn_thread(conn_thread_t *thread);

int init_request_thread(req_thread_t *thread, int conn_epoll);
int spawn_request_thread(req_thread_t *thread);
int close_request_thread(req_thread_t *thread);
void *request_thread_func(void *data_ptr);

/*!
Main process will spawn 2 threads, 1 for socket connection, 1 for request from existing request

Connection threads listen for new connection, upon accpeting a new connection,
the connection is added into the epoll.

*/
int worker_process(int serv_sock, int pipe_fd[2])
{
    printf("worker - %d\n", getpid());

    if(listen_connection(serv_sock) == -1)
    {

        //int code = LISTEN_SOCKET_FAIL;
        int code  = -1;
        write(pipe_fd[1], &code, sizeof(int));
        _Exit(0);
    }

    // Thread variable
    conn_thread_t conn_thrd;
    req_thread_t req_thrd;

    init_conn_thread(&conn_thrd, serv_sock);
    init_request_thread(&req_thrd, conn_thrd.conn_epoll);

    spawn_conn_thread(&conn_thrd);
    spawn_request_thread(&req_thrd);

    while(1) ;

    close_conn_thread(&conn_thrd);
    close_request_thread(&req_thrd);

    close(conn_thrd.conn_epoll);

    _Exit(0);
}

int listen_connection(int serv_sock)
{
    // Listen, wait for new connection
    if(listen(serv_sock, 30) == -1)
    {
        fprintf(stderr, "listen() failed\n");
        return -1;
    }
    printf("Start listen for connection\n");

    return 0;
}




int init_conn_thread(conn_thread_t *thread, int serv_sock)
{
    memset(thread, 0, sizeof(*thread));

    init_flag(&thread->exit_flag);

    // An epoll for existing client socket connection
    int conn_epoll = epoll_create(1000);

    thread->serv_sock = serv_sock;
    thread->conn_epoll = conn_epoll;

    return 0;
}

int spawn_conn_thread(conn_thread_t *thread)
{
    pthread_t thrd;

    //conn_thrd_init_data_t conn_thrd_data = {thread->serv_sock, thread->conn_epoll_fd, thread->exit_flag};

    // Create pthread
    if(pthread_create(&thrd, NULL, connection_thread_func, thread))
    {
        fprintf(stderr, "Error creating thread\n");
        return -1;
    }

    thread->thread = thrd;

    return 0;
}

void *connection_thread_func(void *data_ptr)
{
    if(data_ptr == NULL)
    {
        pthread_exit(NULL);
    }
    /*
    conn_thrd_init_data_t thread_data = *((conn_thrd_init_data_t*)data_ptr);
    int serv_sock = thread_data.serv_sock;
    int conn_epoll_fd = thread_data.conn_epoll;
    */
    conn_thread_t *thread = ((conn_thread_t*)data_ptr);
    int serv_sock = thread->serv_sock;
    int conn_epoll_fd = thread->conn_epoll;

    // Configure epoll_event struct
    struct epoll_event ev;
    ev.events = EPOLLET | EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;

    int quit = 0;

    while(!quit)
    {
        // Accept Connection
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        if(clnt_sock < 0)
        {
            perror("accept() failed ");
            continue;
        }
        printf("Connection accepted\n");

        // Add connection to epoll
        ev.data.fd = clnt_sock;

        if(epoll_ctl(conn_epoll_fd, EPOLL_CTL_ADD, clnt_sock, &ev) != 0)
        {
            // failed
            fprintf(stderr, "epoll_ctl failed, unable to add clnt socket to epoll\n");
            perror("connection_thread_func() ");
        }

        // Check for exit_flag
        quit = check_flag(&thread->exit_flag, 1);
    }

    return NULL;
}


int close_conn_thread(conn_thread_t *thread)
{
    if(thread == NULL)
    {
        return -1;
    }
    if(set_flag(&thread->exit_flag, 1) == -1)
        return -1;

    // Wait for connection thread to finish
    if(pthread_join(thread->thread, NULL))
    {
        fprintf(stderr, "Error joining thread\n");
        return -1;
    }

    destroy_flag(&thread->exit_flag);

    return 0;
}

int init_request_thread(req_thread_t *thread, int conn_epoll)
{
    memset(thread, 0, sizeof(*thread));

    thread->conn_epoll = conn_epoll;
    thread->event_array = calloc(MAX_EVENT_NUM, sizeof(struct epoll_event));
    init_flag(&thread->exit_flag);

    return 0;
}

int spawn_request_thread(req_thread_t *thread)
{
    pthread_t thrd;

    // Create pthread
    if(pthread_create(&thrd, NULL, request_thread_func, thread))
    {
        fprintf(stderr, "Error creating thread\n");
        return -1;

    }

    thread->thread = thrd;

    return 0;
}


void *request_thread_func(void *data_ptr)
{
    /*
    req_thrd_init_data_t data = *((req_thrd_init_data_t*)(data_ptr));
    struct epoll_event *events = data.epoll_event_array;
    int conn_epoll_fd = data.conn_epoll;
    */
    req_thread_t *thread = ((req_thread_t*)(data_ptr));
    struct epoll_event *events = thread->event_array;
    int conn_epoll_fd = thread->conn_epoll;

    int quit = 0;

    while(!quit)
    {
        // wait for something to do...
        int nfds = epoll_wait(conn_epoll_fd, events, MAX_EVENT_NUM, 5);
        if (nfds < 0)
        {
            printf("Error in epoll_wait!\n");
            perror("request_thread_func() ");
            _Exit(-1);
        }

        // for each ready socket
        for(int i = 0; i < nfds; i++) {
            int clnt_sock = events[i].data.fd;
            handle_request(clnt_sock);
        }

        // Check for exit_flag
        quit = check_flag(&thread->exit_flag, 1);
    }

    return NULL;
}

int close_request_thread(req_thread_t *thread)
{
    if(thread == NULL)
    {
        return -1;
    }
    if(set_flag(&thread->exit_flag, 1) == -1)
        return -1;

    // Wait for connection thread to finish
    if(pthread_join(thread->thread, NULL))
    {
        fprintf(stderr, "Error joining thread\n");
        return -1;
    }

    destroy_flag(&thread->exit_flag);

    free(thread->event_array);

    return 0;
}

