#ifndef FEEDER_H
#define FEEDER_H

#include <stdint.h>
#include <unistd.h>

typedef struct {
    int forward_line_fd;
    int backward_line_fd;
    useconds_t backward_time_us;
    useconds_t pause_time_us;
} feeder;

int feeder_init(feeder *feeder, char *chip_name, int forward_line, int backward_line, float backward_time_s, float pause_time_s);
int feeder_feed(feeder *feeder, float feed_time_s);
void feeder_free(feeder *feeder);

#endif // FEEDER_H