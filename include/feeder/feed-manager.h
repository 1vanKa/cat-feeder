#ifndef FEED_CONTROLLER_H
#define FEED_CONTROLLER_H

#include "feeder.h"

typedef struct {
    char *timeofday;
    float feed_time_s;
} feed_item;

typedef struct {
    feeder feeder;
    feed_item *items;
} feed_manager;

int feed_manager_init(feed_manager *manager, const char *config_path);

#endif // FEED_CONTROLLER_H