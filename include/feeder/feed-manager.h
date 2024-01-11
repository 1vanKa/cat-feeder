#ifndef FEED_CONTROLLER_H
#define FEED_CONTROLLER_H

#define __USE_XOPEN
#include <time.h>
#undef __USE_XOPEN

#include "feeder.h"

typedef struct {
    struct tm time;
    double feed_time_s;
} feed_item;

typedef struct {
    feeder feeder;
    feed_item *feeds;
    int feeds_len;
} feed_manager;

int feed_manager_init(feed_manager *manager, const char *config_path);
void feed_manager_free(feed_manager *manager);

int feed_manager_manage(feed_manager *manager);

#endif // FEED_CONTROLLER_H