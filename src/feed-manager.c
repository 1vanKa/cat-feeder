#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "feed-manager.h"
#include "json/cJSON.h"

int tm_hours_mins_to_mins(struct tm *time) {
    return time->tm_hour * 60 + time->tm_min;
}

int feed_item_compare(const void *s1, const void *s2) {
    feed_item *fi1 = (feed_item *)s1;
    feed_item *fi2 = (feed_item *)s2;
    return tm_hours_mins_to_mins(&fi1->time) -
           tm_hours_mins_to_mins(&fi2->time);
}

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
    manager->feeds_len = arr_length;
    memset(manager->feeds, 0, sizeof(*manager->feeds) * arr_length);
    // FIXME: check is time valid in config
    for (int i = 0; i < arr_length; ++i) {
        FREE_ON_NON0(ijson.get_double(json_feed_times[i], "feed_time_s",
                                      &manager->feeds[i].feed_time_s));
        char *timeofday;
        FREE_ON_NON0(
            ijson.get_str(json_feed_times[i], "timeofday", &timeofday));
        strptime(timeofday, "%R", &manager->feeds[i].time);
    }

    qsort(manager->feeds, manager->feeds_len, sizeof(*manager->feeds),
          feed_item_compare);

    for (int i = 0; i < arr_length; ++i) {
        char time[20] = {0};
        strftime(time, 20, "%T", &manager->feeds[i].time);
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

void update_feed_time(struct tm *feed_time, struct tm *src_time) {
    int hours, mins;
    hours = feed_time->tm_hour;
    mins = feed_time->tm_min;
    memcpy(feed_time, src_time, sizeof(*feed_time));
    feed_time->tm_hour = hours;
    feed_time->tm_min = mins;
    feed_time->tm_sec = 0;
}

int find_first_feed(feed_manager *mngr, struct tm *time_info_now) {
    if (mngr->feeds_len < 1) {
        return -1;
    }
    for (int i = 0; i < mngr->feeds_len; ++i) {
        if (tm_hours_mins_to_mins(&mngr->feeds[i].time) >
            tm_hours_mins_to_mins(time_info_now)) {
            update_feed_time(&mngr->feeds[i].time, time_info_now);
            return i;
        }
    }
    update_feed_time(&mngr->feeds->time, time_info_now);
    mngr->feeds->time.tm_mday += 1;
    mktime(&mngr->feeds->time);
    return 0;
}

int feed_manager_manage(feed_manager *manager) {
    useconds_t max_cycle_sleep = 1 * 60 * 1000000;
    time_t time_now, time_feed;
    struct tm time_info;
    int feed_index;
    double secs_before_feed;
    int ret;
    time_feed = time(NULL);
    localtime_r(&time_feed, &time_info);
    feed_index = find_first_feed(manager, &time_info);
    if (feed_index < 0) {
        return -1;
    }
    time_feed = mktime(&manager->feeds[feed_index].time);
    do {
        time_now = time(NULL);
        localtime_r(&time_now, &time_info);
        secs_before_feed = difftime(time_feed, time_now);
        if (secs_before_feed <= 0) {
            char time[20] = {0};
            strftime(time, 20, "%FT%T", &manager->feeds[feed_index].time);
            printf("Feed: %s; Duration: %.3fs\n", time,
                   manager->feeds[feed_index].feed_time_s);
            ret = feeder_feed(&manager->feeder,
                              manager->feeds[feed_index].feed_time_s);
            if (ret) {
                return ret;
            }
            feed_index = (feed_index + 1) % manager->feeds_len;
            update_feed_time(&manager->feeds[feed_index].time, &time_info);
            if (feed_index == 0) {
                manager->feeds[feed_index].time.tm_mday += 1;
            }
            time_feed = mktime(&manager->feeds[feed_index].time);
        } else {
            usleep(secs_before_feed < max_cycle_sleep
                       ? secs_before_feed * 1000000
                       : max_cycle_sleep);
        }
    } while (1);
    return 0;
}
