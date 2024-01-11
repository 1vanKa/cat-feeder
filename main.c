#include <feeder/feed-manager.h>
#include <feeder/feeder.h>
#include <string.h>

int main(/*int argc, char *argv[]*/) {
    feed_manager manager;
    int ret;
    ret = feed_manager_init(&manager, "config.json");
    if (ret) {
        return -1;
    }
    ret = feed_manager_manage(&manager);
    return ret;
}