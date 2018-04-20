#ifndef MPM_H
#define MPM_H

#include <stdlib.h>

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
    Free the mpm_data_t struct
*/
int destory_mpm(mpm_data_t *data);

#endif


