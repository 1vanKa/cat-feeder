#include <cJSON/cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "json.h"

int cJSON_json_parse_file(const_json_ptr2 json, const char *filepath) {
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
    *((const cJSON **)json) =
        cJSON_ParseWithLengthOpts(json_str, json_len, (const char **)&err, 0);
    if (err != NULL && *json == NULL) {
        fprintf(stderr, "Error before: %.*s\n",
                json_len - (int)(err - json_str), err);
    }
    free(json_str);
    if (*json == NULL) {
        return -1;
    }
    return 0;
}

void cJSON_json_free(const_json_ptr2 json) { cJSON_Delete((cJSON *)*json); }

int cJSON_json_get_double(const_json_ptr json, const char *item_name, double *val) {
    cJSON *double_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsNumber(double_json)) {
        return -1;
    }
    *val = double_json->valuedouble;
    return 0;
}

int cJSON_json_get_int(const_json_ptr json, const char *item_name, int *val) {
    cJSON *int_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsNumber(int_json)) {
        return -1;
    }
    *val = int_json->valueint;
    return 0;
}

int cJSON_json_get_str(const_json_ptr json, const char *item_name, const char **str) {
    cJSON *str_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsString(str_json)) {
        return -1;
    }
    *str = malloc(strlen(str_json->valuestring));
    strcpy((char*)*str, str_json->valuestring);
    return 0;
}

/** Returns json_interface which implemented on top of cJSON lib.
 * Do not free returned object, it is a reference to static variable.
*/
json_interface* cJSON_json_interface() {
    static json_interface interface = {.free = NULL};
    if (interface.free == NULL) {
        interface.free = cJSON_json_free;
        interface.parse_file = cJSON_json_parse_file;
        interface.get_double = cJSON_json_get_double;
        interface.get_int = cJSON_json_get_int;
        interface.get_str = cJSON_json_get_str;
    }
    return &interface;
}