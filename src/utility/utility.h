/*!
Utility Functions
*/

#ifndef UTILITY_H
#define UTILITY_H

#include <stdlib.h>

typedef struct raw_request
{
    size_t len;
    char *req_str;
} raw_request_t;

typedef struct request_queue
{
    size_t alloc_size;
} request_queue_t;


char* my_strncpy(char* destination, const char* source, size_t num);

#endif
