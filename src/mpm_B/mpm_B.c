#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>

#include "worker_process.h"
#include "handle_request.h"

#include "mpm_B.h"


int spawn_worker(const int num_worker, int *process_id, int *pipe_fd[2], int serv_sock);
int destory_worker(int pid, int pipe_fd[2]);


int start_mpm_B(mpm_data_t *data, int serv_sock)
{
    // Spawn worker process
    if(spawn_worker(data->num_worker, data->worker_pid, data->to_worker_pipe, serv_sock) == -1)
    {
        return -1;
    }

    return 0;
}


int stop_mpm_B(mpm_data_t *data)
{
    for(int i = 0; i < data->num_worker; i++)
    {
        destory_worker(data->worker_pid[i], data->to_worker_pipe[i]);
    }

    return 0;
}

int spawn_worker(const int num_worker, int *process_id, int *pipe_fd[2], int serv_sock)
{
    // Create pipe to each worker process
    for(int i = 0; i < num_worker; i++)
    {
        pipe(pipe_fd[i]);
    }

    // Forking worker process
    for(int i = 0; i < num_worker; i++)
    {
        pid_t pid = fork();

        // If fork failed
        if(pid == -1)
        {
            // If one fork failed, killed all thread already spawned
            for (int j = 0; j < i; j++)
            {
                kill(process_id[j], SIGKILL);
            }

            // close all pipe
            for (int i = 0; i < num_worker; i++)
            {
                close(pipe_fd[i][0]);
                close(pipe_fd[i][1]);
            }
            return -1;
        }
        // Child
        else if(pid == 0)
        {
            worker_process(serv_sock, pipe_fd[i]);
            _Exit(0);
        }
        // Parent
        else
        {
            process_id[i] = pid;
            continue;
        }
    }

    return 0;
}

int destory_worker(int pid, int pipe_fd[2])
{
    char *msg = "close";
    write(pipe_fd[1], msg, (strlen(msg) + 1) * sizeof(char));

    return 0;
}








