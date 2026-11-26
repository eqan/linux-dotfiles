#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    struct sockaddr_un address = { .sun_family = AF_UNIX };
    snprintf(address.sun_path, sizeof address.sun_path, "/tmp/clipboard-picker-%u.sock", (unsigned)getuid());
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) return 1;
    int result = connect(fd, (struct sockaddr *)&address, sizeof address);
    if (result == 0) (void)write(fd, "toggle", 6);
    close(fd);
    return result == 0 ? 0 : 1;
}
