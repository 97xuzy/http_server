#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <fcntl.h>
#include <errno.h>

#include "../core/parse_request.h"
#include "../core/process_request.h"
#include "../core/generate_response_string.h"


#include "worker_process_A.h"

#define MAX_EVENT_NUM 1000
#define MAX_CON 1000

const char *error_404_response = "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found\r\n";
const char *error_500_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 Internal Server Error\r\n";

int listen_connection(int serv_sock);
int create_conn_epoll(int serv_sock);
int handle_new_client_conn(int conn_epoll, int serv_sock);

int handle_new_request(int clnt_sock);
char *read_request_string(int clnt_sock);


/*!
Main process will spawn 2 threads, 1 for socket connection, 1 for request from existing request

Connection threads listen for new connection, upon accpeting a new connection,
the connection is added into the epoll.

*/
int worker_process_A(int serv_sock, int pipe_fd[2])
{
    printf("worker - %d\n", getpid());

    if(listen_connection(serv_sock) == -1)
    {
        //int code = LISTEN_SOCKET_FAIL;
        int code = -1;
        write(pipe_fd[1], &code, sizeof(int));
        _Exit(0);
    }
    struct epoll_event *event_array = calloc(MAX_EVENT_NUM, sizeof(struct epoll_event));

    // Add Server Socket to Connection Epoll
    int conn_epoll = create_conn_epoll(serv_sock);
    if(conn_epoll == -1)
    {
        _Exit(0);
    }

    while(1)
    {

        // epoll_wait() on Connection Epoll
        int num_fd = epoll_wait(conn_epoll, event_array, MAX_CON, -1);

        for(int i = 0; i < num_fd; i++)
        {
            int clnt_sock = event_array[i].data.fd;

            // If server socket, accept connection
            if(clnt_sock == serv_sock)
            {
                handle_new_client_conn(conn_epoll, serv_sock);
            }
            // If client socket, handle the request
            else
            {
                handle_new_request(clnt_sock);
            }
        }
    }

    free(event_array);

    close(conn_epoll);

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


int create_conn_epoll(int serv_sock)
{
    int conn_epoll = epoll_create(1000);

    // Configure epoll_event struct
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;

    // Add server socket connection to epoll
    ev.data.fd = serv_sock;
    if(epoll_ctl(conn_epoll, EPOLL_CTL_ADD, serv_sock, &ev) != 0)
    {
        // failed
        fprintf(stderr, "epoll_ctl failed, unable to add server socket to epoll\n");
        return -1;
    }

    return conn_epoll;
}

/*!
    Accept the new connection, and add the connection to epoll .
*/
int handle_new_client_conn(int conn_epoll, int serv_sock)
{
    // Configure epoll_event struct
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;

    // Accept the connection
    // Accept can fail fairly often, since multiple worker are compete to accept the connection
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    if(clnt_sock < 0)
    {
        return -1;
    }
    printf("Connection accepted\n");

    // Set client socket as non-blocking
    int flags = fcntl(clnt_sock, F_GETFL, 0);
    fcntl(clnt_sock, F_SETFL, flags | O_NONBLOCK);

    // Add connection to epoll
    ev.data.fd = clnt_sock;
    if(epoll_ctl(conn_epoll, EPOLL_CTL_ADD, clnt_sock, &ev) != 0)
    {
        fprintf(stderr, "epoll_ctl failed, unable to add clnt socket to epoll\n");
        return -1;
    }

    return 0;
}

int handle_new_request(int clnt_sock)
{
    // Read Request
    char *request_str = read_request_string(clnt_sock);
    if(request_str == NULL)
    {
        return -1;
    }

    // Parse Request
    Request request;
    memset(&request, 0, sizeof(Request));
    if(http_request_parser(request_str, &request) != 0)
    {
        fprintf(stderr, "Fail to parse http request\n");
        return -1;
    }

    // Process Request
    Response response;
    memset(&response, 0, sizeof(Response));
    if(process_request(&request, &response) == -1)
    {
        send(clnt_sock, error_404_response, sizeof(*error_404_response) * strlen(error_404_response), 0);

        free(request_str);
        free_request(&request);
        close(clnt_sock);
        return 0;
    }
    free(request_str);
    free_request(&request);

    // Generate response string
    char *response_str = generate_response_string(&response);
    if(response_str == NULL)
    {
        send(clnt_sock, error_500_response, sizeof(*error_500_response) * strlen(error_500_response), 0);

        free_response(&response);
        close(clnt_sock);
        return 0;
    }

    // Send Response
    send(clnt_sock, response_str, sizeof(*response_str) * strlen(response_str), 0);


    free_response(&response);
    free(response_str);
    close(clnt_sock);

    return 0;
}

char *read_request_string(int clnt_sock)
{

    const int buffer_size = 1024;
    char buffer[buffer_size];
    int request_str_len = 0;
    char *request_str = malloc(1024 * sizeof(*request_str));
    if(request_str == NULL)
    {
        return NULL;
    }

    int byte_read = 0;
    do
    {
        //byte_read = read(clnt_sock, buffer, sizeof(buffer));
        byte_read = recv(clnt_sock, buffer, sizeof(buffer), 0);

        if(byte_read < 0)
        {
            free(request_str);
            return NULL;
        }

        memcpy(request_str + request_str_len, buffer, byte_read);
        request_str_len += byte_read;
        request_str = realloc(request_str, request_str_len);

    }
    while(byte_read == buffer_size);

    if(request_str_len == 0 && errno == EWOULDBLOCK)
    {
        return NULL;
    }

    //printf("\"%s\"\n", request_str);

    return request_str;
}



