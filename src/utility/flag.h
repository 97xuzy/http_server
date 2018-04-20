#ifndef FLAG_H
#define FLAG_H

#include <semaphore.h>

typedef struct flag
{
    int flag_val;
    sem_t flag_sem;
} flag_t;

int init_flag(flag_t *flag);
int set_flag(flag_t *flag, int value);
int check_flag(flag_t *flag, int value);
int destroy_flag(flag_t *flag);

#endif