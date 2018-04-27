/*!
    Server will not start if configuration file is not found (even a empty one will do).

    Main process is responsible for create a global configuration that is stored
    in shared meory, each worker process have its own copy of configuration when
    spawned (hence copy the global config to local), thus the configuration can
    be changed on the fly by respawn worker process.

    Global configuration and the shared memory it resides in need to be re-create
    every time a new configuration is loaded.

    When parsing configuration file, if a config item is not present in the file,
    a built-in default value will be used.
*/
#ifndef CONFIG_H
#define CONFIG_H

#include <stdlib.h>
#include <stdbool.h>

#define DEFAULT_CONFIG_FILE_PATH "/var/http_server/config/http_server.conf"

typedef struct config 
{
    /*!
        web_root_path: XXXX
    */
    char *web_root_path;

    bool logging;
    char *log_path;
    int log_level;

    char *error_page_path;

    int worker_num;
} config_t;

config_t* create_global_config(config_t *local);
config_t* copy_global_config_to_local(config_t *global);
int destroy_global_config(config_t *global);
size_t config_size(config_t *config);

/*!
    Read configuration file from the given path,
    parse the file, and store the configuration in
    the config_t struct
*/
int read_config(const char *path, config_t *config);

#endif

