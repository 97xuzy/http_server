#ifndef MPM_H
#define MPM_H

#include <stdlib.h>
#include "../config.h"

typedef struct mpm_data
{
    int num_worker;
    int *worker_pid;
    /*!
        Point to a int[num_worker][2] array
    */
    int **to_worker_pipe;
} mpm_data_t;

/*!
    Init a mpm_data_t struct that will be passed into mpm module
*/ 
mpm_data_t* init_mpm(unsigned int num_worker);

/*!
*/
int start_mpm(mpm_data_t *data, int serv_sock, config_t *config);

/*!
*/
int stop_mpm(mpm_data_t *data);

/*!
    Free the mpm_data_t struct
*/
int destory_mpm(mpm_data_t *data);

#endif


