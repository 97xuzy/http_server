#include <stdio.h>
#include <sys/epoll.h>

int main()
{
    printf("size: %ld\n", sizeof(struct epoll_event));
    return 0;
}
