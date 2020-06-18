// Opening a Pseudo-Terminal
// and Returning the Name of the Slave Device and a File Descriptor

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    int master_fd;
    int slave_fd;
    char *slave_name;

    master_fd = posix_openpt(O_RDWR | O_NOCTTY);
    if (master_fd == -1)
    {
        perror("error to open master pty");
    }
    else
    {
        printf("master fd: %d\n", master_fd);
        if (grantpt(master_fd) == -1)
        {
            perror("error grantpt");
            exit(EXIT_FAILURE);
        }
        else if (unlockpt(master_fd) == -1)
        {
            perror("error unlockpt");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
