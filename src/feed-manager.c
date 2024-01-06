#include <stdio.h>
#include <stdlib.h>

#include "json/json.h"
#include "feed-manager.h"

int feed_manager_init(feed_manager *manager, const char *config_path) {
    json_t *json_config = NULL;
    int ret = 0;
    ret = json_parse_file((const json_t **)&json_config, config_path);
    if (ret) {
        return ret;
    }

    double backward_time_s;
    ret = json_get_double(json_config, "backward_time_s", &backward_time_s);
    if (ret) {
        return ret;
    }

    char *json_str = cJSON_Print(json_config);
    printf("json:\n%s\n", json_str);
    free(json_str);
    cJSON_Delete(json_config);
    return 0;
}
