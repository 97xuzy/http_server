#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>

#include "mpm_A.h"
#include "worker_process_A.h"

int spawn_worker_A(const int num_worker, int *process_id, int *pipe_fd[2], int serv_sock, config_t *global);
int destory_worker_A(int pid, int pipe_fd[2]);

int start_mpm_A(mpm_data_t *data, int serv_sock, config_t *global)
{
    // Set server socket as non-blocking
    int flags = fcntl(serv_sock, F_GETFL, 0);
    if(fcntl(serv_sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        fprintf(stderr, "fcntl() failed, unable to set socket as non-blocking\n");
        return -1;
    }


    // Spawn worker process
    if(spawn_worker_A(data->num_worker, data->worker_pid, data->to_worker_pipe, serv_sock, global) == -1)
    {
        fprintf(stderr, "spawn_worker() failed\n");
        return -1;
    }

    return 0;
}


int spawn_worker_A(const int num_worker, int *process_id, int *pipe_fd[2], int serv_sock, config_t *global)
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
            config_t *local = copy_global_config_to_local(global);
            worker_process_A(serv_sock, pipe_fd[i], local);
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

int stop_mpm_A(mpm_data_t *data)
{
    for(int i = 0; i < data->num_worker; i++)
    {
        destory_worker_A(data->worker_pid[i], data->to_worker_pipe[i]);
    }

    return 0;
}

int destory_worker_A(int pid, int pipe_fd[2])
{
    char *msg = "close";
    write(pipe_fd[1], msg, (strlen(msg) + 1) * sizeof(char));

    return 0;
}





