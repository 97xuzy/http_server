#ifndef WORKER_PROCESS_H
#define WORKER_PROCESS_H

#include "../utility/flag.h"
#include "../config.h"

/********************
 * Worker Process
********************/
int worker_process_A(int serv_sock, int pipe_fd[2], const config_t *local_config);




#endif

