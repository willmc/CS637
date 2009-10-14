#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"


int main(int argc, char *argv[])
{
    int fd;
    int writeRet;
    
    fd = open("foo", (O_CREATE|O_RDWR));
    printf(1,"fd = %d\n", fd);
    writeRet = write(fd, "FOOOOOOOOO", 10);
    printf(1,"writeRet = %d\n", writeRet);

    printf(1, "Checking offset 1: %d\n", (check(fd, 1) > 0 ? 1 : 0));
    printf(1, "Checking offset 128: %d\n", (check(fd, 128) > 0 ? 1 : 0));
    printf(1, "Checking offset 256: %d\n", (check(fd, 256) > 0 ? 1 : 0));
    printf(1, "Checking offset 511: %d\n", (check(fd, 511) > 0 ? 1 : 0));
    printf(1, "Checking offset 512: %d\n", (check(fd, 512) > 0 ? 1 : 0));

    close(fd);

    exit();
}
