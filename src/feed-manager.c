#include <stdio.h>
#include <stdlib.h>

#include "feed-manager.h"
#include "json/cJSON.c"

int parse_config(feed_manager *manager, const char *config_path) {
    json_interface iJson = *cJSON_json_interface();
    json_ptr json_config = NULL;
    int ret = 0;
    ret = iJson.parse_file(&json_config, config_path);
    if (ret) {
        return ret;
    }

    double backward_time_s;
    ret = iJson.get_double(json_config, "backward_time_s", &backward_time_s);
    if (ret) {
        return ret;
    }
    double pause_time_s;
    ret = iJson.get_double(json_config, "pause_time_s", &pause_time_s);
    if (ret) {
        return ret;
    }
    int forward_line;
    ret = iJson.get_int(json_config, "forward_line", &forward_line);
    if (ret) {
        return ret;
    }
    int backward_line;
    ret = iJson.get_int(json_config, "backward_line", &backward_line);
    if (ret) {
        return ret;
    }
    char *gpiochip_name;
    ret = iJson.get_str(json_config, "gpiochip_name",
                        (const char **)&gpiochip_name);
    if (ret) {
        return ret;
    }
    feeder_init(&manager->feeder, gpiochip_name, forward_line, backward_line, backward_time_s, pause_time_s);
    free(gpiochip_name);

    char *json_str = cJSON_Print(json_config);
    printf("json:\n%s\n", json_str);
    free(json_str);
    cJSON_Delete(json_config);
    return 0;
}

int feed_manager_init(feed_manager *manager, const char *config_path) {
    parse_config(manager, config_path);
    return 0;
}
