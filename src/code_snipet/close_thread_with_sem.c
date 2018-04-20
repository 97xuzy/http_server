/*!
Some example code snipet I find online and used in this.

semaphore
http://www.cse.psu.edu/~deh25/cmpsc473/programs/semaphore_sample.c

numerical method for natural log
https://math.stackexchange.com/questions/61279/calculate-logarithms-by-hand

*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <time.h>
#include <pthread.h>    /* required for pthreads */
#include <semaphore.h>  /* required for semaphores */

typedef struct flag
{
    int flag_val;
    sem_t flag_sem;
} flag_t;

// Data passed to and from a Pthread start function 
typedef struct thread_data
{
    int in;	/* used as an input value */
    int out;	/* used as an output value */
    flag_t *exit_flag;
} thread_data_t;


// Thread Routine, run with pthread, compute ln(0) to ln(9999)
void * thread_func(void * arg);
// Work load for the thread, in this case numerically computer natural log
double compute_log(double x);

int init_flag(flag_t *flag);
int set_flag(flag_t *flag, int value);
int check_flag(flag_t *flag, int value);
int destroy_flag(flag_t *flag);


int main(int argc, char * argv[])
{
    pthread_t tid;

    flag_t exit_flag;

    // Thread Data
    thread_data_t a = {0, 0, &exit_flag};
    thread_data_t *b = NULL;

    init_flag(&exit_flag);

    pthread_create(&tid, NULL, thread_func, (void *)&a);

    // sleep for 5 mili sec
    // the work thread might only have time to compute up to ln(43X), compare to ln(9999) when run to completion
    struct timespec interval = {0};
    interval.tv_nsec = 5 * 1000000;
    nanosleep(&interval, NULL);

    // set flag to notify thread to quit
    set_flag(&exit_flag, 1);

    pthread_join(tid, (void **)&b);

    destroy_flag(&exit_flag);

    return 0;
}

void * thread_func(void * arg)
{
    thread_data_t *p = (thread_data_t *)arg;

    int quit = 0;
    int count = 1;

    while(!quit && count < 10000)
    {
        // Work load
        // compute natural log from 1 to 9999
        double log = compute_log((double)count);
        printf("ln(%d) = %lf\n", count, log);
        count ++;

        // Check for exit_flag
        quit = check_flag(p->exit_flag, 1);
    }

    p->out = count;

    return arg;
}

double compute_log(double x)
{
    if(x <= 0.0)
        return -1.0;

    double result = -1.941064448 + (3.529305040 + (-2.461222169 + (1.130626210 + (-0.2887399591 + 0.03110401824 * x) * x) * x) * x) * x;
    return result;
}

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
