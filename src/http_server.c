#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main()
{
    const char url[] = "www.google.com";
    const char ip_addr[] = "127.0.0.1";
    const int port = 80;

    // Create Socket
    int serv_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Bind Socket with IP addr and Port
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;  // IPv4 Addr
    serv_addr.sin_addr.s_addr = inet_addr(ip_addr);
    serv_addr.sin_port = htons(port);
    bind(serv_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    // Listen, wait for user's request
    listen(serv_sock, 20);

    // Accept
    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);
    int clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);

    // Read Request
    char buffer[1024];
    read(clnt_sock,buffer, sizeof(buffer));

    // Parse Request


    // Process Request, And Generate Response


    // Send Response
    char str[] = "Hello World!";
    write(clnt_sock, str, sizeof(str));
   
    // Close Socket
    close(clnt_sock);
    close(serv_sock);

    return 0;
}



