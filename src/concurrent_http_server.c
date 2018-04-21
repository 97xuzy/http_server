#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include "core/http.h"
#include "core/parse_request.h"
#include "core/process_request.h"
#include "core/generate_response_string.h"

#include "mpm.h"

mpm_data_t *data;

int parse_cmd_arg(int argc, char *argv[]);
int server_socket_init(const char *ip_addr, const int port);
void exit_handler(int signum);


int main(int argc, char *argv[])
{
    const int num_worker = 4;

    const int port = parse_cmd_arg(argc, argv);
    const char ip_addr[] = "127.0.0.1";
    if(port != 80 && port < 1024)
    {
        fprintf(stderr, "ERROR, Invalid port number\n");
        return -1;
    }

    //config_t config;
    //read_config(DEFAULT_CONFIG_FILE_PATH, &config);

    // Register Exit Handler (in case of Ctrl-C)
    signal(SIGINT, exit_handler);

    // Create Socket
    int serv_sock = server_socket_init(ip_addr, port);

    // Alloc and Init resource for Multi Process
    data = init_mpm(num_worker);

    // Launch Multi Process
    start_mpm(data, serv_sock);

    while (1)
    {
        ;
    }

    // Kill all worker process
    stop_mpm(data);

    // Free resource for Multi Process
    destory_mpm(data);

    close(serv_sock);

    return 0;
}


int parse_cmd_arg(int argc, char *argv[])
{
    if(argc < 2)
    {
        fprintf(stderr, "ERROR, Need port number, e.g. ./http_server port_number\n");
        exit(-1);
    }
    int port = atoi(argv[1]);
    if(port == 0)
    {
        fprintf(stderr, "ERROR, Unable to parse port number\n");
        exit(-1);
    }
    return port;
}

/*!
    Init a listening socket with a IP addr and a port number.
    If the IP is not given (NULL), then the socket will listen to all inbound 
*/
int server_socket_init(const char *ip_addr, const int port)
{

    // Create Socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(serv_sock < 0)
    {
        perror("ERROR, socket() ");
        _Exit(-1);
    }

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
    if(bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR, bind() ");
        _Exit(-1);
    }

    return serv_sock;
}

/*!
    Exit Handler, upon receive SIGINT signal 
*/
void exit_handler(int signum)
{
    // Kill all worker process
    for(int i = 0; i < data->num_worker; i++)
    {
        kill(data->worker_pid[i], SIGKILL);
    }

}



