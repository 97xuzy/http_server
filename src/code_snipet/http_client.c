/*!

https://www.geeksforgeeks.org/socket-programming-cc/

*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENT_THREAD 10000


const char request_string[] = "GET /index.html HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: Mozilla/5.0 (X11; Fedora; Linux x86_64; rv:59.0) Gecko/20100101 Firefox/59.0\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n"
        "Accept-Language: en-US,en;q=0.5\r\n"
        "Accept-Encoding: gzip, deflate\r\n"
        "DNT: 1\r\n"
        "Connection: keep-alive\r\n"
        "Upgrade-Insecure-Requests: 1\r\n\r\n";

typedef struct thread_init_data
{
    char *ip_addr;
    int port;
    int num_req;

} thread_init_data_t;

int parse_cmd_arg(int argc, char *argv[], int *num_client, int *request_per_client, char *ip_addr, int *port);
void* thread_func(void *data_ptr);
int send_request(int sock);
char *read_response_string(int sock);

int main(int argc, char* argv[])
{

    char ip_addr[64] = {0};
    int port = 0;
    int num_client = 0;
    int request_per_client = 0;

    parse_cmd_arg(argc, argv, &num_client, &request_per_client, ip_addr, &port);

    pthread_t threads[MAX_CLIENT_THREAD];

    thread_init_data_t data = {ip_addr, port, request_per_client};

    for(int i = 0; i < num_client; i++)
    {
        if(pthread_create(&(threads[i]), NULL, thread_func, &data))
        {
            fprintf(stderr, "Error creating thread\n");
            return 1;
        }
    }

    for(int i = 0; i < num_client; i++)
    {
        // Wait for request thread to finish
        if(pthread_join(threads[i], NULL)) {
            fprintf(stderr, "Error joining thread\n");
            return 2;
        }
    }

    return 0;
}

void* thread_func(void *data_ptr)
{

    const thread_init_data_t data = *((thread_init_data_t*)data_ptr);

    const char *ip_addr = data.ip_addr;
    const int port = data.port;
    const int num_req = data.num_req;

    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        pthread_exit(NULL);
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
      
    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr)<=0) 
    {
        printf("\nInvalid address/ Address not supported \n");
        pthread_exit(NULL);
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        pthread_exit(NULL);
    }

    for(int i = 0; i < num_req; i++)
    {
        send_request(sock);

        int milliseconds = 100;
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);

        //read_response_string(sock);
    }

    return NULL;
}

int parse_cmd_arg(int argc, char *argv[], int *num_client, int *request_per_client, char *ip_addr, int *port)
{
    if(argc < 5)
    {
        fprintf(stderr, "Too few arg\n");
        fprintf(stderr, "./http_client num_client req_per_client ip_addr port\n");
        exit(-1);
    }
    int temp1 = atoi(argv[1]);
    if(temp1 <= 0 || temp1 > MAX_CLIENT_THREAD)
    {
        fprintf(stderr, "Number of clients out of range, 1 ~ %d\n", MAX_CLIENT_THREAD);
        exit(-1);
    }

    int temp2 = atoi(argv[2]);
    if(temp2 <= 0 || temp1 > MAX_CLIENT_THREAD)
    {
        fprintf(stderr, "Number of clients out of range, 1 ~ %d\n", MAX_CLIENT_THREAD);
        exit(-1);
    }

    int temp3 = atoi(argv[4]);
    if(temp3 <= 0)
    {
        fprintf(stderr, "Unable to parse port number\n");
        exit(-1);
    }

    *num_client = temp1;
    *request_per_client = temp2;
    strcpy(ip_addr, argv[3]);
    *port = temp3;

    return 0;
}

int send_request(int sock)
{
    send(sock , request_string , strlen(request_string) + 1 , 0 );
    printf("request sent\n");

    return 0;
}

char *read_response_string(int sock)
{

    // Read Request
    char buffer[1024];
    char *request_str = malloc(1024 * sizeof(*request_str));
    int request_str_len = 0;

    int byte_read = 0;
    do
    {
        byte_read = read(sock, buffer, sizeof(buffer));

        if(byte_read < 0) return NULL;

        memcpy(request_str + request_str_len, buffer, byte_read);
        request_str_len += byte_read;
        request_str = realloc(request_str, request_str_len);
        printf("read %d byte\n", byte_read);

    }
    while(byte_read == 1024);

    //printf("\"%s\"\n", request_str);
    printf("response is %ld bytes\n", strlen(request_str));

    free(request_str);

    return request_str;
}
