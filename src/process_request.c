#include "process_request.h"


int classify_request(const Request *request, Response *response)
{
}

int php_request(const Request *request)
{
}

int file_request(const Request *request, Response *response)
{
    const int buffer_size = 4096;
    FILE *fp = NULL;
    char *filename = NULL;
    char *buffer = NULL;

    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        return -1;
    }
    buffer = malloc(buffer_size * sizeof(*buffer));
    if(buffer == NULL)
    {
        return -1;
    }

    fseek(fp, 0, SEEK_END);
    size_t f_size = ftell(fp);
    rewind(fp);

    response->content = malloc(f_size);

    char *temp = response->content;

    while(fread(buffer, sizeof(*buffer), buffer_size, fp) != 0)
    {
        memcpy(temp, buffer, buffer_size);
        temp += buffer_size;
    }

    free(buffer);
    fclose(fp);
}




