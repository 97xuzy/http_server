#ifndef PROCESS_REQUEST_H
#define PROCESS_REQUEST_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../config.h"
#include "http.h"

int process_request(const Request *request, Response *response, const config_t *config);
int error_in_request(int status_code, char *error_file_path, Response *response);

#endif

