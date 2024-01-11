#include <feeder/feed-manager.h>
#include <feeder/feeder.h>
#include <string.h>

int main(int argc, char *argv[]) {
    feed_manager manager;
    feed_manager_init(&manager, "config.json");
    feed_manager_manage(&manager);
    return 0;
}