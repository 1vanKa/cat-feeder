#include <feeder/feeder.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char chip_name[] = "/dev/gpiochip1";
    int line1 = 17;
    int line2 = 27;

    feeder feeder;
    memset(&feeder, 0, sizeof(feeder));
    feeder_init(&feeder, chip_name, line1, line2, 0.5f, 0.5f);
    feeder_feed(&feeder, 1.f);
    feeder_free(&feeder);
    return 0;
}