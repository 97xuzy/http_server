#ifndef PARSE_REQUEST_H
#define PARSE_REQUEST_H

#include "http.h"

int http_request_parser(const char* request_str, Request *request);


#endif
