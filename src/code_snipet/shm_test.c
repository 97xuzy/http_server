/*!
Some example code snipet I find online and used with some modification.

http://www.cse.psu.edu/~deh25/cmpsc473/notes/OSC/Processes/shm.html
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>

int shm_fd = 0;
int child_process();
void sig_handler(int signum);


typedef struct RequestQueue
{
    char *
} RequestQueue;

int main()
{
    const int num_process = 2;
    pid_t pid_array[num_process];

    const int SIZE = 4096;
    const char *name = "OS";
    const char *message0= "Studying ";
    const char *message1= "Operating Systems ";
    const char *message2= "Is Fun!";

    //int shm_fd;
    void *ptr;

    /* create the shared memory segment */
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0666);

    /* configure the size of the shared memory segment */
    ftruncate(shm_fd,SIZE);

    /* now map the shared memory segment in the address space of the process */
    ptr = mmap(0,SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed\n");
        return -1;
    }

    /**
     * Now write to the shared memory region.
     *
     * Note we must increment the value of ptr after each write.
     */
    sprintf(ptr,"%s",message0);
    ptr += strlen(message0);
    sprintf(ptr,"%s",message1);
    ptr += strlen(message1);
    sprintf(ptr,"%s",message2);
    ptr += strlen(message2);

    signal(SIGINT, sig_handler);

    // Spawn child process
    int num = 0;
    for(int i = 0; i < num_process; i++)
    {
        pid_t child_pid = fork();

        // Fork failed
        if(child_pid < 0)
        {
            continue;
        }
        // Child
        else if(child_pid == 0)
        {
            printf("Child Process\n");
            printf("pid : %d\n", getpid());
            child_process();
        }
        // Parent
        else
        {
            //printf("Parent process\n");
            pid_array[i] = child_pid;
            num ++;
        }
    }

    //close(shm_fd);
    //shm_unlink(name);
    while(1);

    return 0;
}


int child_process()
{
	const char *name = "OS";
	const int SIZE = 4096;

	int shm_fd;
	void *ptr;
	int i;

	/* open the shared memory segment */
	shm_fd = shm_open(name, O_RDONLY, 0666);
	if (shm_fd == -1) {
        printf("shared memory failed(pid:%d)\n", getpid());
        exit(-1);
    }

    /* now map the shared memory segment in the address space of the process */
    ptr = mmap(0,SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        printf("Map failed(pid:%d)\n", getpid());
        exit(-1);
    }

    /* now read from the shared memory region */
    printf("(pid:%d)\"%s\"", getpid(), ptr);

    /* remove the shared memory segment */
    /*
    if (shm_unlink(name) == -1) {
        printf("Error removing %s\n",name);
        exit(-1);
	}
    */

    exit(0);
}


void sig_handler(int signum)
{
    printf("Ctrl-C, program terminated\n");
    close(shm_fd);
    shm_unlink("OS");
    exit(0);
}
