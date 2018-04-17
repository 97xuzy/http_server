#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "generate_response_string.h"
#include "http.h"

size_t status_line(const Response *response, char **response_str, size_t alloc_size);
char* reason_phrase_string(int status_code);
size_t general_header(const Response *response, char **response_str, size_t alloc_size);
size_t date_string(char *string, size_t alloc_size);
size_t response_header(const Response *response, char **response_str, size_t alloc_size);
size_t entity_header(const Response *response, char **response_str, size_t alloc_size);
size_t message_body(const Response *response, char **response_str, size_t alloc_size);

char* append_string(char *string1, char *string2);
char* append_2_string(char *string1, char *string2, char *string3);



/*
HTTP/1.1 200 OK
Date: Mon, 23 May 2005 22:38:34 GMT
Content-Type: text/html; charset=UTF-8
Content-Encoding: UTF-8
Content-Length: 138
Last-Modified: Wed, 08 Jan 2003 23:11:55 GMT
Server: Apache/1.3.3.7 (Unix) (Red-Hat/Linux)
ETag: "3f80f-1b6-3e1cb03b"
Accept-Ranges: bytes
Connection: close
*/

/*!

/return Return a pointer to the response_str upon success, return NULL if failed
*/
char* generate_response_string(const Response *response)
{
    if(response == NULL)
    {
        return NULL;
    }

    char *response_str = NULL;
    size_t alloc_size = 2048;

    response_str = calloc(alloc_size, sizeof(*response_str));
    if(response_str == NULL)
    {
        printf("Init alloc for response_str failed\n");
        return NULL;
    }

    alloc_size = status_line(response, &response_str, alloc_size);
    if(alloc_size == 0)
    {
        printf("status_line() failed\n");
        free(response_str);
        return NULL;
    }

    alloc_size = general_header(response, &response_str, alloc_size);
    if(alloc_size == 0)
    {
        printf("general_header() failed\n");
        free(response_str);
        return NULL;
    }

    alloc_size = response_header(response, &response_str, alloc_size);
    if(alloc_size == 0)
    {
        printf("response_header() failed\n");
        free(response_str);
        return NULL;
    }

    alloc_size = entity_header(response, &response_str, alloc_size);
    if(alloc_size == 0)
    {
        printf("entity_header() failed\n");
        free(response_str);
        return NULL;
    }

    alloc_size = message_body(response, &response_str, alloc_size);
    if(alloc_size == 0)
    {
        printf("message_body() failed\n");
        free(response_str);
        return NULL;
    }

    return response_str;
}

/*!

/return Return the number of bytes allocated if succesful, return 0 if failed
*/
size_t status_line(const Response *response, char **response_str, size_t alloc_size)
{
    size_t length = snprintf(NULL, 0, "HTTP/1.1 %d %s\r\n", response->status_code, reason_phrase_string(response->status_code));

    if(alloc_size <= length)
    {
        alloc_size = length * 2;
        *response_str = realloc(*response_str, alloc_size);
        if(*response_str == NULL)
        {
            return 0;
        }
    }
    snprintf(*response_str, alloc_size, "HTTP/1.1 %d %s\r\n", response->status_code, reason_phrase_string(response->status_code));

    return alloc_size;
}

/*!
Date: Mon, 23 May 2005 22:38:34 GMT
*/
size_t general_header(const Response *response, char **response_str, size_t alloc_size)
{
    char date_line[128];

    // Extra space for \r\n
    size_t length = 10*2;

    // Calculate length needed for all applicable general header
    if(response->cache_control != NULL)
        length += strlen(response->cache_control);
    if(response->connection != NULL)
        length += strlen(response->connection);

    length += date_string(date_line, sizeof(date_line));

    if(response->pragma != NULL)
        length += strlen(response->pragma);
    if(response->trailer != NULL)
        length += strlen(response->trailer);
    if(response->transfer_encoding != NULL)
        length += strlen(response->transfer_encoding);
    if(response->connection != NULL)
        length += strlen(response->connection);
    if(response->connection != NULL)
        length += strlen(response->connection);
    if(response->connection != NULL)
        length += strlen(response->connection);
    if(response->connection != NULL)
        length += strlen(response->connection);


    // Realloc if neccessry
    if(alloc_size <= length + strlen(*response_str))
    {
        alloc_size = length + strlen(*response_str);
        alloc_size *= 2;
        *response_str = realloc(*response_str, alloc_size);
        if(*response_str == NULL)
        {
            return 0;
        }
    }

    // Append all applicable general header
    if(response->cache_control != NULL)
        append_2_string(*response_str, response->cache_control, "\r\n");
    if(response->connection != NULL)
        append_2_string(*response_str, response->connection, "\r\n");

    append_2_string(*response_str, date_line, "\r\n");

    if(response->pragma != NULL)
        append_2_string(*response_str, response->pragma, "\r\n");
    if(response->trailer != NULL)
        append_2_string (*response_str, response->trailer, "\r\n");
    if(response->transfer_encoding != NULL)
        append_2_string(*response_str, response->transfer_encoding, "\r\n");
    if(response->upgrade != NULL)
        append_2_string(*response_str, response->upgrade, "\r\n");

    return alloc_size;
}


const char *weekday_str[] = {
    "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
const char *month_str[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

size_t date_string(char *string, size_t alloc_size)
{
    time_t rawtime;
    struct tm * timeinfo;

    time (&rawtime);
    timeinfo = gmtime(&rawtime);

    size_t length = snprintf(NULL, 0, "Date: %s, %d %s %d %d:%d:%d UTC",
            weekday_str[timeinfo->tm_wday], timeinfo->tm_mday, month_str[timeinfo->tm_mon],
            timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    if(alloc_size < length)
        return 0;

    snprintf(string, alloc_size, "Date: %s, %d %s %d %d:%d:%d UTC",
            weekday_str[timeinfo->tm_wday], timeinfo->tm_mday, month_str[timeinfo->tm_mon],
            timeinfo->tm_year+1900, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

    return length;
}

size_t response_header(const Response *response, char **response_str, size_t alloc_size)
{
    // Extra space for \r\n
    size_t length = 9*2;

    // Calculate the length of all the response header line
    if(response->accept_ranges != NULL)
        length += strlen(response->accept_ranges);
    if(response->age != NULL)
        length += strlen(response->age);
    if(response->etag != NULL)
        length += strlen(response->etag);
    if(response->location != NULL)
        length += strlen(response->location);
    if(response->proxy_authenticate != NULL)
        length += strlen(response->proxy_authenticate);
    if(response->retry_after != NULL)
        length += strlen(response->retry_after);
    if(response->server != NULL)
        length += strlen(response->server);
    if(response->vary != NULL)
        length += strlen(response->vary);
    if(response->www_authenticate != NULL)
        length += strlen(response->www_authenticate);


    // Realloc if neccessry
    if(alloc_size <= length + strlen(*response_str))
    {
        alloc_size = length + strlen(*response_str);
        alloc_size *= 2;
        *response_str = realloc(*response_str, alloc_size);
        if(*response_str == NULL)
        {
            return 0;
        }
    }

    // Append the applicable field
    if(response->accept_ranges != NULL)
        append_2_string(*response_str, response->accept_ranges, "\r\n");
    if(response->age != NULL)
        append_2_string(*response_str, response->age, "\r\n");
    if(response->etag != NULL)
        append_2_string(*response_str, response->etag, "\r\n");
    if(response->location != NULL)
        append_2_string(*response_str, response->location, "\r\n");
    if(response->proxy_authenticate != NULL)
        append_2_string(*response_str, response->proxy_authenticate, "\r\n");
    if(response->retry_after != NULL)
        append_2_string(*response_str, response->retry_after, "\r\n");
    if(response->server != NULL)
        append_2_string(*response_str, response->server, "\r\n");
    if(response->vary != NULL)
        append_2_string(*response_str, response->vary, "\r\n");
    if(response->www_authenticate != NULL)
        append_2_string(*response_str, response->www_authenticate, "\r\n");


    return alloc_size;
}

size_t entity_header(const Response *response, char **response_str, size_t alloc_size)
{
    // Extra space for \r\n
    size_t length = 10*2;

    // Calculate the length of all the response header line
    if(response->allow != NULL)
        length += strlen(response->allow);
    if(response->content_encoding != NULL)
        length += strlen(response->content_encoding);
    if(response->content_language != NULL)
        length += strlen(response->content_language);
    if(response->content_length!= NULL)
        length += strlen(response->content_length);
    if(response->content_location != NULL)
        length += strlen(response->content_location);
    if(response->content_md5 != NULL)
        length += strlen(response->content_md5);
    if(response->content_range != NULL)
        length += strlen(response->content_range);
    if(response->content_type != NULL)
        length += strlen(response->content_type);
    if(response->expires!= NULL)
        length += strlen(response->expires);
    if(response->last_modified != NULL)
        length += strlen(response->last_modified);


    // Realloc if neccessry
    if(alloc_size <= length + strlen(*response_str))
    {
        alloc_size = length + strlen(*response_str);
        alloc_size *= 2;
        *response_str = realloc(*response_str, alloc_size);
        if(*response_str == NULL)
        {
            return 0;
        }
    }

    // Append the applicable field
    if(response->allow != NULL)
        append_2_string(*response_str, response->allow, "\r\n");
    if(response->content_encoding != NULL)
        append_2_string(*response_str, response->content_encoding, "\r\n");
    if(response->content_language != NULL)
        append_2_string(*response_str, response->content_language, "\r\n");
    if(response->content_length != NULL)
        append_2_string(*response_str, response->content_length, "\r\n");
    if(response->content_location != NULL)
        append_2_string(*response_str, response->content_location, "\r\n");
    if(response->content_md5 != NULL)
        append_2_string(*response_str, response->content_md5, "\r\n");
    if(response->content_range != NULL)
        append_2_string(*response_str, response->content_range, "\r\n");
    if(response->content_type != NULL)
        append_2_string(*response_str, response->content_type, "\r\n");
    if(response->expires != NULL)
        append_2_string(*response_str, response->expires, "\r\n");
    if(response->last_modified != NULL)
        append_2_string(*response_str, response->last_modified, "\r\n");

    return alloc_size;
}


size_t message_body(const Response *response, char **response_str, size_t alloc_size)
{
    size_t length = strlen(*response_str) + strlen(response->message_body);

    if(alloc_size <= length)
    {
        alloc_size *= 2;
        *response_str = realloc(*response_str, alloc_size);
        if(*response_str == NULL)
        {
            return 0;
        }
    }

    append_2_string(*response_str, "\r\n", response->message_body);

    return alloc_size;
}

char Continue[] = "Continue";
char SwitchingProtocols[] = "Switching Protocols";
char OK[] = "OK";
char Created[] = "Created";
char Accepted[] = "Accepted";
char NonAuthoritativeInformation[] = "Non-Authoritative Information";
char NoContent[] = "No Content";
char ResetContent[] = "Reset Content";
char PartialContent[] = "Partial Content";
char MultipleChoices[] = "Multiple Choices";
char MovedPermanently[] = "Moved Permanently";
char Found[] = "Found";
char SeeOther[] = "See Other";
char NotModified[] = "Not Modified";
char UseProxy[] = "Use Proxy";
char TemporaryRedirect[] = "Temporary Redirect";
char BadRequest[] = "Bad Request";
char Unauthorized[] = "Unauthorized";
char PaymentRequired[] = "Payment Required";
char Forbidden[] = "Forbidden";
char NotFound[] = "Not Found";
char MethodNotAllowed[] = "Method Not Allowed";
char NotAcceptable[] = "Not Acceptable";
char ProxyAuthenticationRequired[] = "Proxy Authentication Required";
char RequestTimeout[] = "Request Time-out";
char Conflict[] = "Conflict";
char Gone[] = "Gone";
char LengthRequired[] = "Length Required";
char PreconditionFailed[] =  "Precondition Failed";
char RequestEntityTooLarge[] = "Request Entity Too Large";
char RequestURITooLarge[] = "Request-URI Too Large";
char UnsupportedMediaType[] = "Unsupported Media Type";
char RequestedRangeNotSatisfiable[] = "Requested range not satisfiable";
char ExpectationFailed[] = "Expectation Failed";
char InternalServerError[] = "Internal Server Error";
char NotImplemented[] =  "Not Implemented";
char BadGateway[] = "Bad Gateway";
char ServiceUnavailable[] = "Service Unavailable";
char GatewayTimeOut[] = "Gateway Time-out";
char HTTPVersionNotSupported[] = "HTTP Version not supported";

char* reason_phrase_string(int status_code)
{
    switch(status_code)
    {
        case 100: return Continue;
        case 101: return SwitchingProtocols;
        case 200: return OK;
        case 201: return Created;
        case 202: return Accepted;
        case 203: return NonAuthoritativeInformation;
        case 204: return NoContent;
        case 205: return ResetContent;
        case 206: return PartialContent;
        case 300: return MultipleChoices;
        case 301: return MovedPermanently;
        case 302: return Found;
        case 303: return SeeOther;
        case 304: return NotModified;
        case 305: return UseProxy;
        case 307: return TemporaryRedirect;
        case 400: return BadRequest;
        case 401: return Unauthorized;
        case 402: return PaymentRequired;
        case 403: return Forbidden;
        case 404: return NotFound;
        case 405: return MethodNotAllowed;
        case 406: return NotAcceptable;
        case 407: return ProxyAuthenticationRequired;
        case 408: return RequestTimeout;
        case 409: return Conflict;
        case 410: return Gone;
        case 411: return LengthRequired;
        case 412: return PreconditionFailed;
        case 413: return RequestEntityTooLarge;
        case 414: return RequestURITooLarge;
        case 415: return UnsupportedMediaType;
        case 416: return RequestedRangeNotSatisfiable;
        case 417: return ExpectationFailed;
        case 500: return InternalServerError;
        case 501: return NotImplemented;
        case 502: return BadGateway;
        case 503: return ServiceUnavailable;
        case 504: return GatewayTimeOut;
        case 505: return HTTPVersionNotSupported;

        default: return NULL;
    }
}

/*!
Append string2 to the end of string1.
Assume the memory allocated for string 1 is enough to hold string1 and string2.
*/
inline char* append_string(char *string1, char *string2)
{
    size_t len1 = strlen(string1);
    //size_t len2 = strlen(string2);

    //char *str = realloc(string1, len1 + len2);
    //strcpy(str, string1);
    strcpy(string1 + len1, string2);

    return string1;
}
/*!
Append string2 and string3 to the end of string1. Hence the resulting string1
is "string1""string2""string3".
Assume the memory allocated for string 1 is enough to hold string1, string2 and string3.
*/
inline char* append_2_string(char *string1, char *string2, char *string3)
{
    append_string(string1, string2);
    append_string(string1, string3);

    return string1;
}





