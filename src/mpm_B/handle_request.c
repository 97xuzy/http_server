#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>

#include "handle_request.h"
#include "../core/parse_request.h"
#include "../core/process_request.h"
#include "../core/generate_response_string.h"

const char *error_404_response = "HTTP/1.1 404 Not Found\r\n\r\n404 Not Found\r\n";
const char *error_500_response = "HTTP/1.1 500 Internal Server Error\r\n\r\n500 Internal Server Error\r\n";

char *read_request_string(int clnt_sock);

int handle_request(int clnt_sock)
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
        return 0;
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
        return 0;
    }

    // Send Response
    write(clnt_sock, response_str, sizeof(*response_str) * (strlen(response_str) + 1));


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

    //printf("\"%s\"\n", request_str);

    return request_str;
}

