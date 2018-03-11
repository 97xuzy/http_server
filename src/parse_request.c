#include "parse_request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char field_names[][20] = {
    "Host",
    "User-Agent",
    "Accept",
    "Accept-Encoding",
    "Connection"
};

int other_lines(const char *line, char **field_name, char **field_value);
int first_line(const char *line, Request *request);
RequestType request_type(const char *string);
int http_version(const char *string);

char* my_strncpy(char* destination, const char* source, size_t num);

int main()
{

    Request req;
    memset(&req, 0, sizeof(req));

/*
    first_line("GET /index.html HTTP/1.1\r\n", &req);
    printf("type:%d\n", req.type);
    printf("ver:%d\n", req.protocol_version);
    printf("path:%s\n", req.path);
    printf("host:%s\n", req.host);

    char *field_name = NULL;
    char *field_value = NULL;
    other_lines("Server: GWS/2.0\r\n", &field_name, &field_value);
    printf("other lines:\n");
    printf("field name: %s\n", field_name);
    printf("field value: %s\n", field_value);
    */

    const char request_str[] = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nConnection: keep-alive\r\n";
    if(http_request_parser(request_str, &req) != 0)
    {
        printf("fail to parse\n");
        return 0;
    }
    printf("--------------------\n");
    printf("prot_ver: %d\n", req.protocol_version);
    printf("path: %s\n", req.path);
    printf("host: %s\n", req.host);

    return 0;
}

/*!
*/
int http_request_parser(const char* request_str, Request *request)
{
    if(request_str == NULL || request == NULL)
        return -1;

    int len = strlen(request_str);
    int line_count = 0;
    int line_start_index = 0;
    int prev_new_line_index = 0;

    // Check if string is at least longer than the min starting line
    // Length of "GET / HTTP/1.1\r\n"
    if (len < 16)
    {
        return -1;
    }

    for(int i = 1; i < len; i++)
    {
        if(request_str[i] == '\n' && request_str[i-1] == '\r')
        {
            char line[i - prev_new_line_index];
            char *field = NULL;
            char *value = NULL;
            memset(line, 0, sizeof(line));
            my_strncpy(line, request_str + prev_new_line_index, i-1 - prev_new_line_index);
            printf("line:\"%s\"\n", line);

            switch(line_count)
            {
                // First Line
                case 0:
                    if(first_line(line, request) != 0)
                    {
                        return -1;
                    }
                    break;
                default:
                    if(other_lines(line, &field, &value) != 0)
                    {
                        printf("other line fail\n");
                        return -1;
                    }
                    if(strcmp(field, "Host") == 0)
                    {
                        request->host = value;
                        free(field);
                    }
                    // If unrecognizable, discard the result, and free the memory
                    else
                    {
                        printf("Unrecognizable field: %s\n", field);
                        free(field);
                        free(value);
                    }
                    break;
            }
            prev_new_line_index = i+1;
            line_count++;
        }
    }
    return 0;
}

/*!
Parse the starting line of the request
allocate memory for request->path

\param line Take in a string which is the first line of the request, the line should NOT have new line at the end, "GET /index.html HTTP/1.1"
\param request A Request struct that contains the info parsed from the line
\return Return 0 upon success
*/
int first_line(const char *line, Request *request)
{
    int space_count = 0;
    int prev_space_index = 0;

    for(int i = 0 ; i < strlen(line); i++)
    {
        if(line[i] == ' ')
        {
            space_count++;
            char temp[i - prev_space_index + 5];
            switch(space_count)
            {
                // Request Type, GET, POST
                case 1:
                    memset(temp, 0, sizeof(temp));
                    my_strncpy(temp, line, i);
                    RequestType type = request_type(temp);
                    request->type = type;
                    break;
                // Path, and HTTP version
                case 2:
                    // Path
                    memset(temp, 0, sizeof(temp));
                    my_strncpy(temp, line + prev_space_index + 1, i - prev_space_index - 1);
                    // Need validation
                    request->path = malloc(strlen(temp) + 1);
                    strcpy(request->path, temp);

                    // Version
                    char temp2[strlen(line) - i];
                    memset(temp2, 0, sizeof(temp2));
                    strcpy(temp2, line+i+1);
                    int ver = http_version(temp2);
                    request->protocol_version = ver;
                    break;
            }
            prev_space_index = i;
        }
    }
    return 0;
}

/*!
Separate the field name and vaule and allocate memory for the field_name, and field_value.
If there is not a ':' in the line string, then return -1 to signify failure

\param line The string contains a line other than the 1st line in the request, the string should NOT end with new line
\param field_name The pointer to the pointer which will hold the address of the memory allocation for field name
\param field_value The pointer to the pointer which will hold the address of the memory allocation for field value
\return Return 0 upon success
*/
int other_lines(const char *line, char **field_name, char **field_value)
{
    if(field_name == NULL || field_value == NULL)
    {
        return -1;
    }

    for(int i = 0 ; i < strlen(line); i++)
    {
        if(line[i] == ':')
        {
            *field_name = malloc(i+1);
            *field_value = malloc(strlen(line) - i);
            my_strncpy(*field_name, line, i);
            strcpy(*field_value, line+i+2);     // +2 due to the space after the ':'

            printf("field_name: %d %s\n", strlen(*field_name), *field_name);
            printf("field_value: %d %s\n", strlen(*field_value), *field_value);

            return 0;
        }
    }
    return -1;
}

/*!
Parse the request type from string
\param string The string like "GET"
\return An enum value of the request type, if unable to parse, return NA (one of enum value)
*/
RequestType request_type(const char *string)
{
    if(string == NULL)
    {
        return NA;
    }

    if(strcmp(string, "GET") == 0)
    {
        return GET;
    }
    else if(strcmp(string, "HEAD") == 0)
    {
        return HEAD;
    }
    else if(strcmp(string, "POST") == 0)
    {
        return POST;
    }
    else if(strcmp(string, "PUT") == 0)
    {
        return PUT;
    }
    else if(strcmp(string, "DELETE") == 0)
    {
        return DELETE;
    }
    else if(strcmp(string, "TRACE") == 0)
    {
        return TRACE;
    }
    else if(strcmp(string, "OPTIONS") == 0)
    {
        return OPTIONS;
    }
    else if(strcmp(string, "CONNECT") == 0)
    {
        return CONNECT;
    }
    else if(strcmp(string, "PATCH") == 0)
    {
        return PATCH;
    }
    else return NA;
}

/*!
\param string The string "HTTP/1.1"
\return The version number of the http protocol as an integer, 1.1 is represent as 11
*/
int http_version(const char *string)
{
    if(string == NULL)
    {
        return -1;
    }
    const char str[] = "HTTP/";
    char buffer[20];

    if(!(strlen(string) - strlen(str) >= 3))
    {
        return -1;
    }

    // Check if start with "HTTP/"
    memset(buffer, 0, sizeof(buffer));
    my_strncpy(buffer, string, strlen(str));

    if(strcmp(buffer, str) != 0)
        return -1;

    // Extract the version number
    memset(buffer, 0, sizeof(buffer));
    my_strncpy(buffer, string+strlen(str), strlen(string) - strlen(str));
    char *dot = buffer;
    while(*dot++)
    {
        if(*dot == '.')
        {
            *dot = '\0';
        }
    }
    int i1 = atoi(buffer);
    int i2 = atoi(dot);

    for(int i = 0; i < strlen(string) - strlen(str)-2; i++)
    {
        i1 *= 10;
    }
    i1 += i2;

    return i1;
}


/*!
Copy a number of characters from source to destination.
A null character will be added at the end of destination string.
(strncpy from stdlib doesnt guarantee the null character under all condition)
which means the destination char array must have space for at least (num + 1)
bytes for the operation to be complete correctly
*/
char* my_strncpy(char* destination, const char* source, size_t num)
{
    for(size_t i = 0; i < num; i++)
    {
        *destination = *source;
        destination++;
        source++;
    }
    *destination = '\0';
    return destination;
}



