#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "http.h"

int process_request(const Request *request, Response *response);
int file_request(const Request *request, const char *web_root_path, Response *response);

#endif

