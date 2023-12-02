#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/led_matrix", O_WRONLY);

    char buf[32];

    for (int i = 0; i < 32; i++) {
        // Pattern
        buf[i] = 0b01010101;
    }

    write(fd, buf, 32);

    close(fd);
    return 0;
}

