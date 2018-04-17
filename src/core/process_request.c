#include "process_request.h"

#define FILE_REQUEST 100

int classify_request(const Request *request, Response *response);
int file_request(const Request *request, const char *web_root_path, Response *response);


int process_request(const Request *request, Response *response)
{
    // Dispatch request, Invoke handler
    switch(classify_request(request, response))
    {
        case FILE_REQUEST:
            if(file_request(request, "/var/http_server/web_root", response) == -1)
            {
                return -1;
            }
            break;
        default:
            return -1;
    }
    response->status_code = StatusCode_OK;
    return 0;
}


int classify_request(const Request *request, Response *response)
{
    return FILE_REQUEST;
}

/*!
/param web_root_path Path should NOT end with '\'
*/
int file_request(const Request *request, const char *web_root_path, Response *response)
{
    if(request == NULL || web_root_path == NULL || response == NULL)
    {
        return -1;
    }
    const size_t buffer_size = 32;
    FILE *fp = NULL;
    char *filename = NULL;
    char *buffer = NULL;

    // Combine web_root_path and request->path to generate filename
    filename = calloc(strlen(web_root_path) + strlen(request->path) + 5, sizeof(*filename));
    strcpy(filename, web_root_path);
    strcpy(filename + strlen(web_root_path), request->path);
    //printf("filename: \"%s\"\n", filename);

    // Open file
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "Unable to open file\n");
        free(filename);
        return -1;
    }

    // Allocate read buffer
    buffer = calloc(buffer_size, sizeof(*buffer));
    if(buffer == NULL)
    {
        fprintf(stderr, "malloc for buffer failed\n");
        free(filename);
        fclose(fp);
        return -1;
    }

    char *message_body = calloc(buffer_size, sizeof(*message_body));
    if(message_body == NULL)
    {
        fprintf(stderr, "malloc for response->content failed\n");
        free(buffer);
        free(filename);
        fclose(fp);
        return -1;
    }

    // Buffered read of the request file
    size_t allocated_size = buffer_size;
    size_t total_byte_read = 0;
    size_t byte_read = 0;
    while((byte_read = fread(buffer, sizeof(*buffer), buffer_size, fp)) > 0)
    {
        if(total_byte_read >= allocated_size)
        {
            allocated_size *= 2;
            message_body = realloc(message_body, allocated_size);
            if(message_body == NULL)
            {
                fprintf(stderr, "realloc for response->message_body failed\n");
                free(buffer);
                free(filename);
                fclose(fp);
                return -1;
            }
        }
        memcpy(message_body + total_byte_read, buffer, byte_read);
        total_byte_read += byte_read;
    }
    *(message_body + allocated_size) = '\0';
    response->message_body = message_body;

    // If error occur
    if(ferror(fp))
    {
        fprintf(stderr, "file error occur\n");
        free(response->message_body);
        free(buffer);
        free(filename);
        fclose(fp);
        return -1;
    }
    //printf("response->message_body: \"%s\"\n", response->message_body);

    free(buffer);
    free(filename);
    fclose(fp);

    return 0;

}


#ifdef TEST

int test_file_request()
{
    Request request;
    Response response;
    const char web_root_path[64] = "~/";

    if(file_request(&request, web_root_path, &response) != 0)
    {
        fprintf(stderr, "file_request() failed");
        return FALSE;
    }
}

#endif

