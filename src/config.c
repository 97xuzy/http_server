#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/mman.h>


#include "config.h"

void default_config(config_t *config);
int parse_config_file(char *file_str, config_t *config);
int parse_line(char *line, int len, config_t *config);
int parse_field(config_t *config, char *field, int field_len, char *value);
char *read_file(FILE *fp);

config_t* create_global_config(config_t *local)
{
    // Create shared memory with mmap MAP_ANONYMOUS,
    // thus the memory can only be shared among main and worker processes
    void *ptr = mmap(NULL, config_size(local), PROT_READ | PROT_WRITE,
        MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if(ptr == MAP_FAILED)
    {
        return NULL;
    }

    // Copy the struct
    memcpy(ptr, local, sizeof(*local));

    // Copy memory linked by ptr in the struct
    // and re-link the memory with ptrs in the struct
    config_t *global = ptr;
    ptrdiff_t offset = sizeof(*local);
    memcpy(ptr + offset, local->web_root_path, strlen(local->web_root_path) + 1);
    global->web_root_path = ptr + offset;
    offset += strlen(local->web_root_path) + 1;

    memcpy(ptr + offset, local->log_path, strlen(local->log_path) + 1);
    global->log_path = ptr + offset;
    offset += strlen(local->log_path) + 1;

    memcpy(ptr + offset, local->error_page_path, strlen(local->error_page_path) + 1);
    global->error_page_path = ptr + offset;
    //offset += strlen(local->error_page_path) + 1;

    return global;
}

config_t* copy_global_config_to_local(config_t *global)
{
    size_t size = config_size(global);
    void *ptr = calloc(1, size);
    if(ptr == NULL)
    {
        return NULL;
    }
    config_t *local = ptr;

    // Copy memory
    memcpy(ptr, global, size);

    // Re-link the memory with ptrs in the struct
    ptrdiff_t offset = sizeof(*local);

    local->web_root_path = ptr + offset;
    offset += strlen(global->web_root_path) + 1;

    local->log_path = ptr + offset;
    offset += strlen(global->log_path) + 1;

    local->error_page_path = ptr + offset;
    //offset += strlen(global->error_page_path) + 1;

    return local;
}

int destroy_global_config(config_t *global)
{
    munmap(global, config_size(global));
    return 0;
}

size_t config_size(config_t *config)
{
    size_t size = sizeof(config_t);

    size += strlen(config->web_root_path) + 1;
    size += strlen(config->log_path) + 1;
    size += strlen(config->error_page_path) + 1;

    return size;
}

void default_config(config_t *config)
{
    static char *web_root_path = "/var/http_server/web_root";
    static char *log_path = "/var/http_server/log";
    static char *error_page_path = "/var/http_server/error_page";

    config->web_root_path = web_root_path;
    config->logging = true;
    config->log_path = log_path;
    config->log_level = 1;
    config->worker_num = 4;
    config->error_page_path = error_page_path;
}

/*!
    Read configuration file from the given path,
    parse the file, and store the configuration in
    the config_t struct
*/
int read_config(const char *path, config_t *config)
{
    FILE *fp = fopen(path, "r");
    if(fp == NULL)
    {
        fprintf(stderr, "ERROR, Fail to open config file\n");
        return -1;
    }

    char *file_str = read_file(fp);
    if(file_str == NULL)
    {
        fprintf(stderr, "ERROR, Fail to read config file\n");
        return -1;
    }

    // Fill with default value
    default_config(config);

    if(parse_config_file(file_str, config) == -1)
    {
        fprintf(stderr, "ERROR, Fail to parse config file\n");
        return -1;
    }

    free(file_str);
    fclose(fp);

    return 0;
}

/*!
    Parse the configuration file in a line-by-line fashion 
*/
int parse_config_file(char *file_str, config_t *config)
{
    memset(config, 0, sizeof(*config));

    int line_num = 1;
    int prev_index = 0;

    for(int i = 0; i < strlen(file_str); i++)
    {
        if(file_str[i] == '\n' || file_str[i] == '\0')
        {
            if(parse_line(file_str + prev_index, i - prev_index, config) == -1)
            {
                fprintf(stderr, "ERROR, fail to parse line No.%d\n", line_num);
                return -1;
            }
            prev_index = i+1;
            line_num++;
        }
    }

    return 0;
}

/*!
    Parse line, ignore comment line, empty line and line
    longer than 200 char.
*/
int parse_line(char *line, int len, config_t *config)
{
    // Return 1, if the line is comment or empty
    if(line[0] == '#' || line[0] == '\n')
    {
        return 1;
    }

    // Ignore line longer than 200
    if(len > 200)
    {
        return 0;
    }

    for(int i = 0; i < len; i++)
    {
        if(line[i] == ':')
        {
            char *value = calloc(len - i + 1, 1);
            if(value == NULL)
            {
                return -1;
            }
            memcpy(value, line + i + 2, len - i - 2);

            if(parse_field(config, line, i, value) == -1)
            {
                free(value);
            }
            return 0;
        }
    }

    // fail
    return -1;
}

/*!
    Check if the field name is recognizable. if the
    field value is to be converted into other types,
    the value string will be discard/free after convertion.
*/
int parse_field(config_t *config, char *field, int field_len, char *value)
{
    char field_str[field_len + 1];

    memset(field_str, 0, sizeof(field_str));
    memcpy(field_str, field, field_len);

    if(strcmp(field_str, "web_root_path") == 0)
    {
        config->web_root_path = value;
        return 0;
    }
    else if(strcmp(field_str, "logging") == 0)
    {
        config->logging = false;

        if(strcmp(value, "on") == 0)
        {
            config->logging = true;
        }
        free(value);
        return 0;
    }
    else if(strcmp(field_str, "log_path") == 0)
    {
        config->log_path = value;
        return 0;
    }
    else if(strcmp(field_str, "log_level") == 0)
    {
        config->log_level = atoi(value);
        free(value);
        return 0;
    }
    else if(strcmp(field_str, "error_page_path") == 0)
    {
        config->error_page_path = value;
        return 0;
    }
    else if(strcmp(field_str, "worker_num") == 0)
    {
        config->worker_num = atoi(value);
        free(value);
        return 0;
    }


    return -1;
}

/*!
    Read file into a single string, and return the string 
*/
char *read_file(FILE *fp)
{
    const int buffer_size = 1024;
    char buffer[buffer_size];
    size_t alloc_size = buffer_size;
    size_t byte_read = 0;
    size_t file_str_len = 0;
    char *file_str = NULL;

    memset(buffer, 0, sizeof(*buffer) * alloc_size);

    file_str = calloc(buffer_size, sizeof(*buffer));
    if(file_str == NULL)
    {
        return NULL;
    }

    do
    {
        size_t byte_read = fread(buffer, sizeof(*buffer), buffer_size, fp);

        if(byte_read < 0)
        {
            free(file_str);
            return NULL;
        }

        memcpy(file_str + file_str_len, buffer, byte_read);
        file_str_len += byte_read;

        if(alloc_size <= file_str_len)
        {
            alloc_size *= 2;
            file_str = realloc(file_str, alloc_size);
            if(file_str == NULL)
            {
                free(file_str);
                return NULL;
            }
        }
    }
    while(byte_read == buffer_size);

    return file_str;
}


#ifdef TEST

const char conf_str[] =
    "########################################\n"
    "web_root_path: /var/http_server/web_root\n"
    "\n"
    "########################################\n"
    "# Logging\n"
    "########################################\n"
    "logging: on\n"
    "log_path: /var/http_server/log\n"
    "log_level: 1\n"
    "\n"
    "########################################\n"
    "# Error\n"
    "########################################\n"
    "\n"
    "#\n"
    "# Error page for file request\n"
    "#\n"
    "error_page_path: /var/http_server/error_page\n"
    "\n"
    "worker_num: 4\n";

int main()
{
    const char filename[] = "config_test.txt";

    FILE *fp = fopen(filename, "w+");
    if(fp == NULL)
    {
        fprintf(stderr, "NON-TEST-ERROR, fail to open file\n");
    }
    fwrite(conf_str, strlen(conf_str), 1, fp);

    fclose(fp);

    config_t config;
    read_config(filename, &config);

    if(strcmp(config.web_root_path, "/var/http_server/web_root") != 0)
    {
        fprintf(stderr, "ERROR, fail to parse \"web_root_path\"\n");
    }
    if(! config.logging)
    {
        fprintf(stderr, "ERROR, fail to parse \"logging\"\n");
    }
    if(strcmp(config.log_path, "/var/http_server/log") != 0)
    {
        fprintf(stderr, "ERROR, fail to parse \"log_path\"\n");
    }
    if(config.log_level != 1)
    {
        fprintf(stderr, "ERROR, fail to parse \"log_level\"\n");
    }
    if(strcmp(config.error_page_path, "/var/http_server/error_page") != 0)
    {
        fprintf(stderr, "ERROR, fail to parse \"error_page_path\"\n");
    }
    if(config.worker_num != 4)
    {
        fprintf(stderr, "ERROR, fail to parse \"worker_num\"\n");
    }

    remove(filename);

    return 0;
}

#endif

