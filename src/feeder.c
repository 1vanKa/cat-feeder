#include <errno.h>
#include <fcntl.h>
#include <linux/gpio.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "feeder.h"

const struct gpio_v2_line_values high = {.bits = 1, .mask = 1};
const struct gpio_v2_line_values low = {.bits = 0, .mask = 1};

int feeder_init(feeder *feeder, char *chip_name, int forward_line,
                int backward_line, float backward_time_s, float pause_time_s) {
    int chip_fd = open(chip_name, O_RDWR);
    if (chip_fd < 0) {
        fprintf(stderr, "Failed to open: %s\n", chip_name);
        return -1;
    }

    struct gpio_v2_line_request request;
    memset(&request, 0, sizeof(request));
    request.offsets[0] = forward_line;
    request.num_lines = 1;
    struct gpio_v2_line_config lineConfig;
    memset(&lineConfig, 0, sizeof(lineConfig));
    lineConfig.flags = GPIO_V2_LINE_FLAG_OUTPUT;
    request.config = lineConfig;
    strcpy(request.consumer, "Feeder");

    if (ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &request) == -1) {
        fprintf(stderr, "Failed to get line: %d\n", request.offsets[0]);
        feeder_free(feeder);
        close(chip_fd);
        return -1;
    }

    feeder->forward_line_fd = request.fd;
    request.fd = 0;
    request.offsets[0] = backward_line;

    if (ioctl(chip_fd, GPIO_V2_GET_LINE_IOCTL, &request) == -1) {
        fprintf(stderr, "Failed to get line: %d\n", request.offsets[0]);
        feeder_free(feeder);
        close(chip_fd);
        return -1;
    }

    feeder->backward_line_fd = request.fd;

    close(chip_fd);

    if (backward_time_s < 0) {
        feeder->backward_time_us = 0;
    } else {
        feeder->backward_time_us = (useconds_t)(backward_time_s * 1000000);
    }
    if (pause_time_s < 0) {
        feeder->pause_time_us = 0;
    } else {
        feeder->pause_time_us = (useconds_t)(pause_time_s * 1000000);
    }
    return 0;
}

int set_line(int line_fd, uint8_t val) {
    if (ioctl(line_fd, GPIO_V2_LINE_SET_VALUES_IOCTL, val ? &high : &low) ==
        -1) {
        fprintf(stderr, "Failed to set line. Error: %s\n", strerror(errno));
        return -1;
    }
    return 0;
}

int set_high(int line_fd) { return set_line(line_fd, 1); }

int set_low(int line_fd) { return set_line(line_fd, 0); }

int feeder_feed(feeder *feeder, float feed_time_s) {
    if (set_high(feeder->backward_line_fd)) {
        return -1;
    }
    usleep(feeder->backward_time_us);
    if (set_low(feeder->backward_line_fd)) {
        return -1;
    }
    usleep(feeder->pause_time_us);
    if (set_high(feeder->forward_line_fd)) {
        return -1;
    }
    usleep((useconds_t)(feed_time_s * 1000000));
    if (set_low(feeder->forward_line_fd)) {
        return -1;
    }
    return 0;
}

void feeder_free(feeder *feeder) {
    if (feeder->forward_line_fd > 0) {
        close(feeder->forward_line_fd);
    }
    if (feeder->backward_line_fd > 0) {
        close(feeder->backward_line_fd);
    }
}
