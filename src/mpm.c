#include "mpm.h"

#if defined(MPM_A)
    #include "mpm_A/mpm_A.h"
#elif defined(MPM_B)
    #include "mpm_B/mpm_B.h"
#else
    #error "No MPM selected"
#endif

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

int start_mpm(mpm_data_t *data, int serv_sock)
{
#if defined(MPM_A)
    return start_mpm_A(data, serv_sock);
#elif defined(MPM_B)
    return start_mpm_B(data, serv_sock);
#else
    #error "No MPM selected"
#endif
}

int stop_mpm(mpm_data_t *data)
{
#if defined(MPM_A)
    return stop_mpm_A(data);
#elif defined(MPM_B)
    return stop_mpm_B(data);
#else
    #error "No MPM selected"
#endif
}

/*!
    Free the mpm_data_t struct
*/
int destory_mpm(mpm_data_t *data)
{
    free(data);
    return 0;
}



