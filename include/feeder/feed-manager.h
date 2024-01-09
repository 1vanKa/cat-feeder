#ifndef FEED_CONTROLLER_H
#define FEED_CONTROLLER_H

#include "feeder.h"

typedef struct {
    char *timeofday;
    double feed_time_s;
} feed_item;

typedef struct {
    feeder feeder;
    feed_item *feeds;
    int items_len;
} feed_manager;

int feed_manager_init(feed_manager *manager, const char *config_path);
void feed_manager_free(feed_manager *manager);

#endif // FEED_CONTROLLER_H