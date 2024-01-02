#include <cJSON/cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "feed-manager.h"

int feed_manager_init(feed_manager *manager, const char *config_path) {
    int json_file = open(config_path, O_RDONLY);
    if (json_file < 0) {
        return -1;
    }
    struct stat json_file_stat;
    stat(config_path, &json_file_stat);
    char *json_str = malloc(json_file_stat.st_size * sizeof(char));
    int json_len = read(json_file, json_str, json_file_stat.st_size);
    char *err = NULL;
    cJSON *json =
        cJSON_ParseWithLengthOpts(json_str, json_len, (const char **)&err, 0);
    if (err != NULL && json == NULL) {
        fprintf(stderr, "Error at: %.*s\n", json_len - (err - json_str),
                err);
    }
    free(json_str);
    if (json == NULL) {
        return -1;
    }
    json_str = cJSON_Print(json);
    printf("json:\n%s\n", json_str);
    free(json_str);
    cJSON_Delete(json);
    return 0;
}
