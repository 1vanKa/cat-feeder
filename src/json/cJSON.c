#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "json.h"

int json_parse_file(const json_t **json, const char *filepath) {
    int json_file = open(filepath, O_RDONLY);
    if (json_file < 0) {
        return -1;
    }
    struct stat json_file_stat;
    stat(filepath, &json_file_stat);
    char *json_str = malloc(json_file_stat.st_size * sizeof(char));
    int json_len = read(json_file, json_str, json_file_stat.st_size);
    close(json_file);

    char *err = NULL;
    *json =
        cJSON_ParseWithLengthOpts(json_str, json_len, (const char **)&err, 0);
    if (err != NULL && *json == NULL) {
        fprintf(stderr, "Error before: %.*s\n", json_len - (err - json_str),
                err);
    }
    free(json_str);
    if (*json == NULL) {
        return -1;
    }
    return 0;
}

int json_get_double(const json_t *json, const char *item_name, double *val) {
    cJSON *double_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsNumber(double_json)) {
        return -1;
    }
    *val = double_json->valuedouble;
    return 0;
}

int json_get_int(const json_t *json, const char *item_name, int *val) {
    cJSON *int_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsNumber(int_json)) {
        return -1;
    }
    *val = int_json->valueint;
    return 0;
}