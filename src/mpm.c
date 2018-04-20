#include "mpm.h"


/*!
    Init a mpm_data_t struct that will be passed into mpm module
*/ 
mpm_data_t* init_mpm(unsigned int num_worker)
{
    size_t size_alloc = sizeof(mpm_data_t) + sizeof(int) * num_worker * 3;

    void *ptr = calloc(1, size_alloc);
    mpm_data_t *data = ptr;
    data->num_worker = num_worker;
    data->worker_pid = ptr + sizeof(int);
    data->to_worker_pipe = (void*)data->worker_pid + sizeof(int) * num_worker;

    return ptr;
}

/*!
    Free the mpm_data_t struct
*/
int destory_mpm(mpm_data_t *data)
{
    free(data);
    return 0;
}



