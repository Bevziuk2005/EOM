#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Фінальне завдання: копіювання файла лише через open/read/write/close.
   ./file_copy source destination */

#define BUFFER_SIZE 4096

/* пише рівно count байтів, навіть якщо write() за раз записав менше */
static int write_all(int fd, const char *buf, size_t count)
{
    size_t total = 0;

    while (total < count) {
        ssize_t written = write(fd, buf + total, count - total);

        if (written == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        total += (size_t)written;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(
            stderr,
            "Usage: %s <source> <destination>\n",
            argv[0]
        );

        return EXIT_FAILURE;
    }

    int source = open(argv[1], O_RDONLY);

    if (source == -1) {
        perror("open source");
        return EXIT_FAILURE;
    }

    int destination = open(
        argv[2],
        O_WRONLY | O_CREAT | O_TRUNC,
        0644
    );

    if (destination == -1) {
        perror("open destination");
        close(source);              /* не лишаємо source відкритим */
        return EXIT_FAILURE;
    }

    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    int status = EXIT_SUCCESS;

    while ((bytes_read =
                read(source, buffer, sizeof(buffer))) != 0) {

        if (bytes_read == -1) {
            if (errno == EINTR) {
                continue;
            }
            perror("read");
            status = EXIT_FAILURE;
            break;
        }

        if (write_all(destination, buffer, (size_t)bytes_read) == -1) {
            perror("write");
            status = EXIT_FAILURE;
            break;
        }
    }

    if (close(source) == -1) {
        perror("close source");
        status = EXIT_FAILURE;
    }

    if (close(destination) == -1) {
        perror("close destination");
        status = EXIT_FAILURE;
    }

    return status;
}
