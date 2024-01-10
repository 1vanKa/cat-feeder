#include <stdio.h>
#include <stdlib.h>

#include "feed-manager.h"
#include "json/cJSON.c"

#define FREE_ON_NON0(func_call)                                                \
    ret = func_call;                                                           \
    if (ret) {                                                                 \
        feed_manager_free(manager);                                            \
        return ret;                                                            \
    }

int parse_config(feed_manager *manager, const char *config_path) {
    memset(manager, 0, sizeof(*manager));
    json_interface ijson = *cJSON_json_impl();
    json_ptr json_config = NULL;
    int ret = 0;

    FREE_ON_NON0(ijson.parse_file(&json_config, config_path));

    double backward_time_s;
    FREE_ON_NON0(
        ijson.get_double(json_config, "backward_time_s", &backward_time_s));
    double pause_time_s;
    FREE_ON_NON0(ijson.get_double(json_config, "pause_time_s", &pause_time_s));
    int forward_line;
    FREE_ON_NON0(ijson.get_int(json_config, "forward_line", &forward_line));
    int backward_line;
    FREE_ON_NON0(ijson.get_int(json_config, "backward_line", &backward_line));
    char *gpiochip_name = NULL;
    FREE_ON_NON0(ijson.get_str(json_config, "gpiochip_name", &gpiochip_name));
    feeder_init(&manager->feeder, gpiochip_name, forward_line, backward_line,
                backward_time_s, pause_time_s);
    free(gpiochip_name);

    json_ptr *json_feed_times;
    int arr_length;
    FREE_ON_NON0(ijson.get_arr_items_pointers(json_config, "feed_times",
                                              &json_feed_times, &arr_length));
    manager->feeds = malloc(sizeof(*manager->feeds) * arr_length);
    if (!manager->feeds) {
        feed_manager_free(manager);
        return -1;
    }
    memset(manager->feeds, 0, sizeof(*manager->feeds) * arr_length);
    for (int i = 0; i < arr_length; ++i) {
        FREE_ON_NON0(ijson.get_double(json_feed_times[i], "feed_time_s",
                                      &manager->feeds[i].feed_time_s));
        char *timeofday;
        FREE_ON_NON0(
            ijson.get_str(json_feed_times[i], "timeofday", &timeofday));
        strptime(timeofday, "%R", &manager->feeds[i].time);
        char time[20] = {0};
        strftime(time, 20, "%FT%T", &manager->feeds[i].time);
        printf("Time: %s; Feed: %.3fs\n", time, manager->feeds[i].feed_time_s);
    }

    // char *json_str = cJSON_Print(json_config);
    // printf("json:\n%s\n", json_str);
    // free(json_str);
    ijson.free(json_config);
    return 0;
}

#undef FREE_ON_NON0

int feed_manager_init(feed_manager *manager, const char *config_path) {
    parse_config(manager, config_path);
    return 0;
}

void feed_manager_free(feed_manager *manager) {
    feeder_free(&manager->feeder);
    free(manager->feeds);
}

int feed_manager_manage(feed_manager *manager) {
    time_t time_now, time_prev;
    struct tm time_info;
    time_prev = time(NULL);
    do {
        time_now = time(NULL);
        localtime_r(&time_now, &time_info);
        // difftime();

        time_prev = time_now;
    } while (1);
}
