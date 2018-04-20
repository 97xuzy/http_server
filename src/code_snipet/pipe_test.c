#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
 
int main()
{
    // parent to child 1
    int pc1_fd[2];
    // child 1 to parent
    int c1p_fd[2];

    char parent_in[80] = {0};
    char parent_out[80] = "p-c: 123";
    char child_in[80] = {0};
    char child_out[80] = "c-p: 456";

    pipe(pc1_fd);
    pipe(c1p_fd);

    pid_t pid = fork();

    if(pid < 0)
    {
        printf("fork failed!\n");
        exit(-1);
    }
    // Child
    else if(pid == 0)
    {
        close(c1p_fd[0]);
        close(pc1_fd[1]);

        char message[10] = "free";
        write(c1p_fd[1], message, strlen(message) + 1);

        char buffer[128] = {0};
        while(read(pc1_fd[0], buffer, sizeof(buffer)) > 0)
        {
            printf("child buffer: %s\n", buffer);
        }

    }
    // Parent
    else
    {
        close(c1p_fd[1]);
        close(pc1_fd[0]);

        char buffer[10];
        do
        {
            read(c1p_fd[0], buffer, sizeof(buffer));
            printf("parent buffer: %s\n", buffer);
        }
        while(strcmp(buffer, "free") != 0);

        char request[128] = "GET /index.html HTTP/1.1\r\n";

        write(pc1_fd[1], request, strlen(request) + 1);
    }

    return 0;
}
