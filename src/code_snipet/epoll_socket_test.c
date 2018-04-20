/*!
Some example code snipet I find online and used in this.

epoll:
https://kovyrin.net/2006/04/13/epoll-asynchronous-network-programming/

pthread:
http://timmurphy.org/2010/05/04/pthreads-in-c-a-minimal-working-example/
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>

#define MAX_CON 1000


/*!
Data pass into connection thread when spawning one
*/
typedef struct conn_thrd_init_data
{
    int serv_sock;
    int conn_epoll;
} conn_thrd_init_data_t;

/*!
Data pass into request thread when spawning one
*/
typedef struct req_thrd_init_data
{
    int conn_epoll;
    struct epoll_event *epoll_event_array;
} req_thrd_init_data_t;


int server_socket_init(const char *ip_addr, const int port);
int listen_connection(int serv_sock);
void *connection_thread_func(void *conn_thrd_init_data_ptr);
void *request_thread_func(void *epoll_event_array);
int handle_io_on_socket(int clnt_sock);

/*!
Main process will spawn 2 threads, 1 for socket connection, 1 for request from existing request

Connection threads listen for new connection, upon accpeting a new connection,
the connection is added into the epoll.

*/
int main()
{
    /* Socket Init */
    int serv_sock = server_socket_init("127.0.0.1", 8080);

    listen_connection(serv_sock);

    struct epoll_event *events = calloc(MAX_CON, sizeof(struct epoll_event));


    // Thread variable
    pthread_t accept_conn_thread;
    pthread_t req_thread;

    // An epoll for existing client socket connection
    int conn_epoll_fd = epoll_create(1000);




    conn_thrd_init_data_t conn_thrd_data = {serv_sock, conn_epoll_fd};

    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&accept_conn_thread, NULL, connection_thread_func, &conn_thrd_data))
    {
        fprintf(stderr, "Error creating thread\n");
        return 1;

    }

    req_thrd_init_data_t req_thr_data = {conn_epoll_fd, events};
    /* create a second thread which executes inc_x(&x) */
    if(pthread_create(&req_thread, NULL, request_thread_func, &req_thr_data))
    {
        fprintf(stderr, "Error creating thread\n");
        return 1;

    }

    while(1) ;

    // Wait for connection thread to finish
    if(pthread_join(accept_conn_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }
    // Wait for request thread to finish
    if(pthread_join(req_thread, NULL)) {
        fprintf(stderr, "Error joining thread\n");
        return 2;
    }

    close(conn_epoll_fd);


    return 0;
}


int server_socket_init(const char *ip_addr, const int port)
{

    // Create Socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Bind Socket with IP addr and Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4 Addr

    // If no specific ip is provided, listen to all address
    if(ip_addr == NULL)
    {
        serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
        serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    }

    serv_addr.sin_port = htons(port);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    return serv_sock;
}

int listen_connection(int serv_sock)
{
    // Listen, wait for new connection
    printf("Start listen for connection\n");
    listen(serv_sock, 20);

    return 0;
}


void *connection_thread_func(void *data_ptr)
{
    conn_thrd_init_data_t thread_data = *((conn_thrd_init_data_t*)data_ptr);
    int serv_sock = thread_data.serv_sock;
    int conn_epoll_fd = thread_data.conn_epoll;

    // Configure epoll_event struct
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;

    while(1)
    {
        // Accept Connection
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        printf("Connection accepted\n");

        // Add connection to epoll
        ev.data.fd = clnt_sock;
        if(epoll_ctl(conn_epoll_fd, EPOLL_CTL_ADD, clnt_sock, &ev) != 0)
        {
            // failed
            fprintf(stderr, "epoll_ctl failed, unable to add clnt socket to epoll\n");
        }
    }

}

void *request_thread_func(void *data_ptr)
{
    req_thrd_init_data_t data = *((req_thrd_init_data_t*)(data_ptr));
    struct epoll_event *events = data.epoll_event_array;
    int conn_epoll_fd = data.conn_epoll;

    while(1)
    {
        // wait for something to do...
        int nfds = epoll_wait(conn_epoll_fd, events, MAX_CON, 5);
        if (nfds < 0)
        {
            printf("Error in epoll_wait!\n");
            exit(-1);
        }

        // for each ready socket
        for(int i = 0; i < nfds; i++) {
            int clnt_sock = events[i].data.fd;
            handle_io_on_socket(clnt_sock);
        }
    }
}

int handle_io_on_socket(int clnt_sock)
{
    char buffer[512];
    while(read(clnt_sock, buffer, sizeof(buffer)) > 0)
    {
        //printf("buffer:\"%s\"\n", buffer);
        printf("buffer: %ld bytes\n", strlen(buffer));
    }

    return 0;
}



