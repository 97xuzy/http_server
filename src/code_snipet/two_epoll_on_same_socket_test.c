#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <sys/epoll.h>
#include <pthread.h>


#define MAX_EVENT_NUM 100


int parse_cmd_arg(int argc, char *argv[]);
int server_socket_init(const char *ip_addr, const int port);
int listen_connection(int serv_sock);

const char *error_404_response = "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found\r\n";

int main(int argc, char *argv[])
{
    const int port = parse_cmd_arg(argc, argv);
    const char ip_addr[] = "127.0.0.1";
    if(port != 80 && port < 1024)
    {
        printf("Invalid port number\n");
        return -1;
    }

    // Create Socket
    int serv_sock = server_socket_init(ip_addr, port);


    int conn_epoll1 = epoll_create(100);
    int conn_epoll2 = epoll_create(100);

    struct epoll_event *event_array1 = calloc(100, sizeof(struct epoll_event));
    struct epoll_event *event_array2 = calloc(100, sizeof(struct epoll_event));

    // Configure epoll_event struct
    struct epoll_event ev;
    ev.events = EPOLLIN | EPOLLPRI | EPOLLERR | EPOLLHUP;

    // Accept Connection
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("Connection accepted\n");

    // Add server socket connection to epoll
    ev.data.fd = serv_sock;
    if(epoll_ctl(conn_epoll1, EPOLL_CTL_ADD, serv_sock, &ev) != 0)
    {
        // failed
        fprintf(stderr, "epoll_ctl failed, unable to add clnt socket to epoll\n");
        return -1;
    }

    if(epoll_ctl(conn_epoll2, EPOLL_CTL_ADD, serv_sock, &ev) != 0)
    {
        // failed
        fprintf(stderr, "epoll_ctl failed, unable to add clnt socket to epoll\n");
        return -1;
    }


    // Listen
    listen_connection(serv_sock);

    int nfds = 0;

    while(1)
    {


        // wait for something to do...
        nfds = epoll_wait(conn_epoll1, event_array1, MAX_EVENT_NUM, 0);
        if (nfds < 0)
        {
            printf("Error in epoll_wait!\n");
            perror("request_thread_func() ");
            _Exit(-1);
        }

        // for each ready socket
        for(int i = 0; i < nfds; i++) {
            int serv_sock = event_array1[i].data.fd;


            struct sockaddr_in clnt_addr;
            socklen_t clnt_addr_size = sizeof(clnt_addr);
            clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if(clnt_sock < 0)
            {
                perror("accept() failed ");
                continue;
            }
            printf("Conn Accepted-1\n");
            write(clnt_sock, error_404_response, sizeof(*error_404_response) * strlen(error_404_response));
            close(clnt_sock);
        }

        // wait for something to do...
        nfds = epoll_wait(conn_epoll2, event_array2, MAX_EVENT_NUM, 0);
        if (nfds < 0)
        {
            printf("Error in epoll_wait!\n");
            perror("request_thread_func() ");
            _Exit(-1);
        }

        // for each ready socket
        for(int i = 0; i < nfds; i++) {
            int serv_sock = event_array2[i].data.fd;


            struct sockaddr_in clnt_addr;
            socklen_t clnt_addr_size = sizeof(clnt_addr);
            clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
            if(clnt_sock < 0)
            {
                perror("accept() failed ");
                continue;
            }
            printf("Conn Accepted-2\n");
            write(clnt_sock, error_404_response, sizeof(*error_404_response) * strlen(error_404_response));
            close(clnt_sock);
        }


    }
    close(serv_sock);

    return 0;
}


int parse_cmd_arg(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Need port number\n");
        printf("./http_server port\n");
        exit(-1);
    }
    int port = atoi(argv[1]);
    if(port == 0)
    {
        printf("Unable to parse port number\n");
        exit(-1);
    }
    return port;
}

int server_socket_init(const char *ip_addr, const int port)
{

    // Create Socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Bind Socket with IP addr and Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4 Addr
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
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


