#include <feeder/feed-manager.h>
#include <feeder/feeder.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char chip_name[] = "/dev/gpiochip0";
    int line1 = 17;
    int line2 = 27;

    feed_manager manager;
    feed_manager_init(&manager, "config.json");

    feeder feeder;
    memset(&feeder, 0, sizeof(feeder));
    int ret = feeder_init(&feeder, chip_name, line1, line2, 0.5f, 0.5f);
    if (ret) {
        return ret;
    }
    for (int i = 0; i < 1; ++i)
    if (ret = feeder_feed(&feeder, 1.f)) {
        return ret;
    }
    feeder_free(&feeder);
    return 0;
}