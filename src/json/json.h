#ifndef JSON_H
#define JSON_H

typedef void* json_ptr;
typedef const json_ptr const_json_ptr;
typedef const json_ptr* const_json_ptr2;

typedef void json_free(const_json_ptr2 json);
typedef int json_parse_file(const_json_ptr2 json, const char *filepath);

typedef int json_get_double(const_json_ptr json, const char *item_name, double *val);
typedef int json_get_int(const_json_ptr json, const char *item_name, int *val);
typedef int json_get_str(const_json_ptr json, const char *item_name, const char **str);


typedef struct {
    json_free *free;
    json_parse_file *parse_file;

    json_get_double *get_double;
    json_get_int *get_int;
    json_get_str *get_str;
} json_interface;

#endif // JSON_H