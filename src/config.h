#ifndef CONFIG_H
#define CONFIG_H

const char config_file_path[] = "/tmp/http_server/http_server.conf";

typedef struct Config 
{
    char *web_root_path;
    int worker_num;
} Config;

int read_config_file(Config *config);

#endif

