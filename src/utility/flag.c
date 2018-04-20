#include <stdlib.h>
#include <stdio.h>

#include "flag.h"



int init_flag(flag_t *flag)
{
    flag->flag_val = 0;

    if (sem_init(&flag->flag_sem, 0, 1) == -1)
    {
        fprintf(stderr, "sem_init() failed\n");
        return -1;
    }
    return 0;
}

int set_flag(flag_t *flag, int value)
{
    if(flag == NULL)
        return -1;
    sem_wait(&flag->flag_sem);         /* start of critical section */
    flag->flag_val = value;
    sem_post(&flag->flag_sem);         /* end of critical section */
    return 0;
}

int check_flag(flag_t *flag, int value)
{
    int flag_val = 0;
    if(flag == NULL)
        return -1;
    sem_wait(&flag->flag_sem);         /* start of critical section */
    flag_val = flag->flag_val;
    sem_post(&flag->flag_sem);         /* end of critical section */

    return flag_val == value;

}

int destroy_flag(flag_t *flag)
{
    if(flag == NULL)
        return -1;
    if(sem_destroy(&flag->flag_sem) != 0)
    {
        fprintf(stderr, "sem_destroy() failed\n");
        return -1;
    }
    return 0;
}

