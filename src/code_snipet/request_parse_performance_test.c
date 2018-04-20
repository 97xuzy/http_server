#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include "../core/http.h"
#include "../core/parse_request.h"
#include "../core/process_request.h"
#include "../core/generate_response_string.h"

char *read_request_string(int clnt_sock);

int main(int argc, char *argv[])
{
/*
    // Read Request
    char *request_str = read_request_string(clnt_sock);
    if(request_str == NULL)
    {
        return -1;
    }
*/
    char *request_str = "GET / HTTP/1.1\r\n"
        "Host: localhost:8081\r\n"
        "User-Agent: Mozilla/5.0 (X11; Fedora; Linux x86_64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "DNT: 1\r\n"
        "Connection: keep-alive\r\n"
        "Upgrade-Insecure-Requests: 1\r\n\r\n";


    // Parse Request
    Request request;
    memset(&request, 0, sizeof(Request));
    if(http_request_parser(request_str, &request) != 0)
    {
        printf("Fail to parse http request\n");
        return -1;
    }
/*
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
*/
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
