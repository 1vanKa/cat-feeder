#include <cJSON/cJSON.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "json.h"

int cJSON_json_parse_file(json_ptr2 json, const char *filepath) {
    int json_file = open(filepath, O_RDONLY);
    if (json_file < 0) {
        return -1;
    }
    struct stat json_file_stat;
    stat(filepath, &json_file_stat);
    char *json_str = malloc(json_file_stat.st_size * sizeof(char));
    if (!json_str) {
        return -1;
    }
    int json_len = read(json_file, json_str, json_file_stat.st_size);
    close(json_file);

    char *err = NULL;
    *json =
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

void cJSON_json_free(json_ptr json) { cJSON_Delete(json); }

int cJSON_json_get_double(const_json_ptr json, const char *item_name,
                          double *val) {
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

int cJSON_json_get_str(const_json_ptr json, const char *item_name,
                       char **str) {
    cJSON *str_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsString(str_json)) {
        return -1;
    }
    *str = malloc(strlen(str_json->valuestring));
    if (!str) {
        return -1;
    }
    strcpy(*str, str_json->valuestring);
    return 0;
}

int cJSON_json_get_obj(const_json_ptr json, const char *item_name,
                       json_ptr2 json_item) {
    cJSON *obj_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsObject(obj_json)) {
        return -1;
    }
    *json_item = obj_json;
    return 0;
}

int cJSON_json_get_arr_obj(const_json_ptr json, const char *item_name,
                           json_ptr2 json_arr_obj) {
    cJSON *arr_json = cJSON_GetObjectItemCaseSensitive(json, item_name);
    if (!cJSON_IsArray(arr_json)) {
        return -1;
    }
    *json_arr_obj = arr_json;
    return 0;
}

int cJSON_json_get_arr_item(const_json_ptr json, int index,
                            json_ptr2 json_arr_item) {
    if (!cJSON_IsArray(json)) {
        return -1;
    }
    cJSON *arr_item_json = cJSON_GetArrayItem(json, index);
    if (arr_item_json == NULL) {
        return -1;
    }
    *json_arr_item = arr_item_json;
    return 0;
}

int cJSON_json_get_arr_items_pointers(const_json_ptr json,
                                      const char *arr_item_name,
                                      json_ptr3 json_arr_pointers,
                                      int *arr_length) {
    cJSON *arr_obj_json;
    int ret =
        cJSON_json_get_arr_obj(json, arr_item_name, (json_ptr2)&arr_obj_json);
    if (ret) {
        return ret;
    }
    *arr_length = cJSON_GetArraySize(arr_obj_json);
    *json_arr_pointers = malloc(sizeof(**json_arr_pointers) * *arr_length);
    if (!*json_arr_pointers) {
        return -1;
    }
    cJSON *element;
    int i = 0;
    cJSON_ArrayForEach(element, (cJSON *)arr_obj_json) {
        (*json_arr_pointers)[i++] = element;
    }
    return 0;
}

/**
 * Returns json_interface which implemented on top of cJSON lib.
 * Do not free returned object, it is a reference to static variable.
 */
json_interface *cJSON_json_impl() {
    static json_interface interface = {.free = NULL};
    if (interface.free == NULL) {
        interface.free = cJSON_json_free;
        interface.parse_file = cJSON_json_parse_file;
        interface.get_double = cJSON_json_get_double;
        interface.get_int = cJSON_json_get_int;
        interface.get_str = cJSON_json_get_str;
        interface.get_obj = cJSON_json_get_obj;
        interface.get_arr_obj = cJSON_json_get_arr_obj;
        interface.get_arr_item = cJSON_json_get_arr_item;
        interface.get_arr_items_pointers = cJSON_json_get_arr_items_pointers;
    }
    return &interface;
}