#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "core/http.h"
#include "core/parse_request.h"
#include "core/process_request.h"
#include "core/generate_response_string.h"

int parse_cmd_arg(int argc, char *argv[]);
int server_socket_init(const char *ip_addr, const int port);
int listen_connection(int serv_sock);
char *read_request_string(int clnt_sock);

const char *error_404_response = "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found\r\n";
const char *error_500_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 Internal Server Error\r\n";

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

    // Listen
    listen_connection(serv_sock);

    while(1)
    {
        // Accept
        struct sockaddr_in clnt_addr;
        socklen_t clnt_addr_size = sizeof(clnt_addr);
        int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
        printf("Connection accepted\n");

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
            printf("Fail to parse http request\n");
            return -1;
        }

        // Process Request
        Response response;
        memset(&response, 0, sizeof(Response));
        if(process_request(&request, &response) == -1)
        {
            write(clnt_sock, error_404_response, sizeof(*error_404_response) * strlen(error_404_response));

            free(request_str);
            free_request(&request);
            close(clnt_sock);
            continue;
        }
        free(request_str);
        free_request(&request);

        // Generate response string
        char *response_str = generate_response_string(&response);
        if(response_str == NULL)
        {
            write(clnt_sock, error_500_response, sizeof(*error_500_response) * strlen(error_500_response));

            free_response(&response);
            close(clnt_sock);
            continue;
        }

        // Send Response
        write(clnt_sock, response_str, sizeof(*response_str) * strlen(response_str));


        free_response(&response);
        free(response_str);
       
        // Close Socket
        // Connection is closed after the response being sent.
        // New connection needs to be esatblished for every request
        close(clnt_sock);

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


char *read_request_string(int clnt_sock)
{

    // Read Request
    char buffer[1024];
    char *request_str = malloc(1024 * sizeof(*request_str));
    int request_str_len = 0;

    int byte_read = 0;
    do
    {
        byte_read = read(clnt_sock, buffer, sizeof(buffer));

        if(byte_read < 0) return NULL;

        memcpy(request_str + request_str_len, buffer, byte_read);
        request_str_len += byte_read;
        request_str = realloc(request_str, request_str_len);
        printf("read %d byte\n", byte_read);

    }
    while(byte_read == 1024);

    printf("\"%s\"\n", request_str);

    return request_str;
}
