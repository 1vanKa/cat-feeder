#include <cJSON/cJSON.h>

typedef cJSON json_t;

int json_parse_file(const json_t **json, const char *filepath);

int json_get_double(const json_t *json, const char *item_name, double *val);
int json_get_int(const json_t *json, const char *item_name, int *val);
