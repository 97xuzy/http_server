/*!
Some example code snipet I find online and used with some modification.

https://www.geeksforgeeks.org/named-pipe-fifo-example-c-program/
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
 
int main()
{
    int fd;
 
    // FIFO file path
    char * myfifo = "/tmp/to_wp1";
 
    // Creating the named file(FIFO)
    // mkfifo(<pathname>, <permission>)
    mkfifo(myfifo, 0666);
 
    char parent_in[80] = {0};
    char parent_out[80] = "p-c: 123";
    char child_in[80] = {0};
    char child_out[80] = "c-p: 456";

    pid_t pid = fork();

    if(pid < 0)
    {
        printf("fork failed!\n");
        exit(-1);
    }
    // Child
    else if(pid == 0)
    {
        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        read(fd, child_in, sizeof(child_in));

        // Print the read message
        printf("From Parent: %s\n", child_in);
        close(fd);

        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, child_out, strlen(child_out)+1);
        close(fd);
    }
    // Parent
    else
    {
        // Open FIFO for write only
        fd = open(myfifo, O_WRONLY);

        // Write the input arr2ing on FIFO
        // and close it
        write(fd, parent_out, strlen(parent_out)+1);
        close(fd);

        // Open FIFO for Read only
        fd = open(myfifo, O_RDONLY);

        // Read from FIFO
        while(read(fd, parent_in, sizeof(parent_in)) > 0)
        {

        // Print the read message
        printf("From Child: %s\n",parent_in);
        }
        close(fd);
    }

    return 0;
}
