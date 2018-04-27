#include "parse_request.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "../utility/utility.h"


int other_lines(const char *line, char **field_name, char **field_value);
int first_line(const char *line, Request *request);
int read_in_field(Request *request, char *field_name, char *field_value);
RequestType request_type(const char *string);
int http_version(const char *string);

/*!
*/
int http_request_parser(const char* request_str, Request *request)
{
    if(request_str == NULL || request == NULL)
        return -1;

    int len = strlen(request_str);
    int line_count = 0;
    int prev_new_line_index = 0;

    // Check if string is at least longer than the min starting line
    // Length of "GET / HTTP/1.1\r\n"
    if (len < 16)
    {
        return -1;
    }

    // Parse line by line
    for(int i = 1; i < len; i++)
    {
        // Each line should end with /r/n (CRLF)
        if(request_str[i] == '\n' && request_str[i-1] == '\r')
        {
            char line[i - prev_new_line_index];
            char *field = NULL;
            char *value = NULL;
            memset(line, 0, sizeof(line));
            my_strncpy(line, request_str + prev_new_line_index, i-1 - prev_new_line_index);
            //printf("line (%ld):\"%s\"\n", strlen(line), line);

            switch(line_count)
            {
                // First Line
                case 0:
                    if(first_line(line, request) != 0)
                    {
                        return -1;
                    }
                    break;
                // Other Line
                default:
                    // If come across the empty line after all the header field lines
                    if(strlen(line) == 0)
                    {
                        request->message_body = malloc(strlen(request_str + prev_new_line_index));
                        if(request->message_body == NULL)
                        {
                            return -1;
                        }
                        break;
                        break;
                    }
                    if(other_lines(line, &field, &value) != 0)
                    {
                        //printf("other line fail\n");
                        return -1;
                    }
                    if (read_in_field(request, field, value) == -1)
                    {
                        break;
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
            if(*field_name == NULL) return -1;
            *field_value = malloc(strlen(line) - i);
            if(*field_value == NULL) return -1;

            memset(*field_name, 0, i+1);
            memset(*field_value, 0, strlen(line) - i);

            my_strncpy(*field_name, line, i);

            my_strncpy(*field_value,
                    line+i+2, // +2 due to the space after the ':'
                    strlen(line + i + 2));  // strlen() of the rest of the string, since there is no \r\n at the end

            return 0;
        }
    }
    return -1;
}

/*!
if the field_name is recognizable, memory storing field_name will be free, since
it is no longer needed.
If the field_name is NOT recognizable, then free both field_name, and field_value
to discard the content.
\return Return 0 if the field_name is recognizable, return -1 otherwise
*/
int read_in_field(Request *request, char *field_name, char *field_value)
{
    if(request == NULL || field_name == NULL || field_value == NULL)
    {
        return -1;
    }

    // Request Header Fields.
    if (strcmp(field_name, "Accept") == 0)
    {
        request->accept = field_value;
    }
    else if (strcmp(field_name, "Accept-Charset") == 0)
    {
        request->accept_charset = field_value;
    }
    else if (strcmp(field_name, "Accept-Encoding") == 0)
    {
        request->accept_encoding = field_value;
    }
    else if (strcmp(field_name, "Accept-Language") == 0)
    {
        request->accept_language = field_value;
    }
    else if (strcmp(field_name, "Authorization") == 0)
    {
        request->authorization = field_value;
    }
    else if (strcmp(field_name, "Expect") == 0)
    {
        request->expect = field_value;
    }
    else if (strcmp(field_name, "From") == 0)
    {
        request->from = field_value;
    }
    else if (strcmp(field_name, "Host") == 0)
    {
        request->host = field_value;
    }
    else if (strcmp(field_name, "If-Match") == 0)
    {
        request->if_match = field_value;
    }
    else if (strcmp(field_name, "If-Modified-Since") == 0)
    {
        request->if_modified_since = field_value;
    }
    else if (strcmp(field_name, "If-None-Match") == 0)
    {
        request->if_none_match = field_value;
    }
    else if (strcmp(field_name, "If-Range") == 0)
    {
        request->if_range = field_value;
    }
    else if (strcmp(field_name, "If-Unmodified-Since") == 0)
    {
        request->if_unmodified_since = field_value;
    }
    else if (strcmp(field_name, "Max-Forwards") == 0)
    {
        request->max_forwards = field_value;
    }
    else if (strcmp(field_name, "Proxy-Authorization") == 0)
    {
        request->proxy_authorization = field_value;
    }
    else if (strcmp(field_name, "Range") == 0)
    {
        request->range = field_value;
    }
    else if (strcmp(field_name, "Referer") == 0)
    {
        request->referer = field_value;
    }
    else if (strcmp(field_name, "TE") == 0)
    {
        request->TE = field_value;
    }
    else if (strcmp(field_name, "User-Agent") == 0)
    {
        request->user_agent = field_value;
    }

    // General Header Fields
    else if (strcmp(field_name, "Cache-Control") == 0)
    {
        request->cache_control = field_value;
    }
    else if (strcmp(field_name, "Connection") == 0)
    {
        request->connection = field_value;
    }
    else if (strcmp(field_name, "Date") == 0)
    {
        request->date = field_value;
    }
    else if (strcmp(field_name, "Pragma") == 0)
    {
        request->pragma = field_value;
    }
    else if (strcmp(field_name, "Trailer") == 0)
    {
        request->trailer = field_value;
    }
    else if (strcmp(field_name, "Transfer-Encoding") == 0)
    {
        request->transfer_encoding = field_value;
    }
    else if (strcmp(field_name, "Upgrade") == 0)
    {
        request->upgrade = field_value;
    }
    else if (strcmp(field_name, "Via") == 0)
    {
        request->via = field_value;
    }
    else if (strcmp(field_name, "Warning") == 0)
    {
        request->warning = field_value;
    }

    // Entity Header Fields
    else if (strcmp(field_name, "Allow") == 0)
    {
        request->allow = field_value;
    }
    else if (strcmp(field_name, "Content-Encoding") == 0)
    {
        request->content_encoding = field_value;
    }
    else if (strcmp(field_name, "Content-Language") == 0)
    {
        request->content_language = field_value;
    }
    else if (strcmp(field_name, "Content-Length") == 0)
    {
        request->content_length = field_value;
    }
    else if (strcmp(field_name, "Content-Range") == 0)
    {
        request->content_range = field_value;
    }
    else if (strcmp(field_name, "Content-MD5") == 0)
    {
        request->content_md5 = field_value;
    }
    else if (strcmp(field_name, "Content-Range") == 0)
    {
        request->content_range = field_value;
    }
    else if (strcmp(field_name, "Content-Type") == 0)
    {
        request->content_type = field_value;
    }
    else if (strcmp(field_name, "Expires") == 0)
    {
        request->expires = field_value;
    }
    else if (strcmp(field_name, "Last-Modified") == 0)
    {
        request->last_modified = field_value;
    }

    // Not recognizable
    else
    {
        // if field_name is NOT recognizable, the field is ignored,
        // hence free the memeory allocated for both field_name
        // and field_value

        //printf("Unrecognizable field: %s\n", field_name);
        free(field_name);
        free(field_value);
        return -1;
    }

    // if the field_name is recognizable, then memory allocated for
    // field_name is no longer in use, as the memory for field_value
    // is transfered to the designated pointer of the Request struct.
    free(field_name);
    return 0;

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



#ifdef TEST

bool test_http_request_parser()
{
    //printf("\n--------------------\n");
    printf("Test http_request_parser()\n");
    //printf("--------------------\n\n");

    Request req;
    memset(&req, 0, sizeof(req));

    const char request_str[] = "GET /index.html HTTP/1.1\r\nHost: www.example.com\r\nConnection: keep-alive\r\n";
    if(http_request_parser(request_str, &req) != 0)
    {
        printf("fail to parse\n");
        return false;
    }
    printf("prot_ver: \"%d\"\n", req.protocol_version);
    printf("host: \"%s\"\n", req.host);

    if(strcmp(req.path, "/index.html") != 0)
    {
        printf("path: \"%s\"\n", req.path);
        return false;
    }

    return true;
}

bool test_other_lines()
{
    //printf("\n--------------------\n");
    printf("Test other_line()\n");
    //printf("--------------------\n\n");

    Request req;
    char *field_name = NULL;
    char *field_value = NULL;
    memset(&req, 0, sizeof(req));

    if(other_lines("Server: GWS/2.0", &field_name, &field_value) != 0)
    {
        printf("fail to parse\n");
        return false;
    }

    if(strcmp(field_name, "Server") != 0)
    {
        printf("field name: \"%s\"\n", field_name);
        return false;
    }

    if(strcmp(field_value, "GWS/2.0") != 0)
    {
        printf("field value: \"%s\"\n", field_value);
        return false;
    }

    field_name = NULL;
    field_value = NULL;
    memset(&req, 0, sizeof(req));

    if(other_lines("Server GWS/2.0", &field_name, &field_value) == 0)
    {
        printf("Should fail when no \':\', instead of success\n");
        return false;
    }


    return true;
}

bool test_first_line()
{
    //printf("\n--------------------\n");
    printf("Test first_line()\n");
    //printf("--------------------\n\n");

    Request req;
    memset(&req, 0, sizeof(req));

    if(first_line("GET /index.html HTTP/1.1", &req) != 0)
    {
        printf("fail to parse\n");
        return false;
    }

    if(req.type != GET)
    {
        printf("type: \"%d\"\n", req.type);
        return false;
    }
    if(req.protocol_version != 11)
    {
        printf("ver: \"%d\"\n", req.protocol_version);
        return false;
    }
    if(strcmp(req.path, "/index.html") != 0)
    {
        printf("path: \"%s\"\n", req.path);
        return false;
    }

    return true;

}

bool test_read_in_field(Request *request, char *field_name, char *field_value)
{
    printf("\n--------------------\n");
    printf("Test read_in_field()\n");
    printf("--------------------\n\n");
    ;
    return true;
}

bool test_request_type(const char *string)
{
    printf("\n--------------------\n");
    printf("Test request_type()\n");
    printf("--------------------\n\n");
    ;
    return true;
}

bool test_http_version(const char *string)
{
    printf("\n--------------------\n");
    printf("Test http_version()\n");
    printf("--------------------\n\n");
    ;
    return true;
}


// Main Function for Test
int main()
{
    test_other_lines();
    test_first_line();
    //test_my_strncpy();

    return 0;
}

#endif
