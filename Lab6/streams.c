#include <stdio.h>
#include <unistd.h>

/* Завдання 14: stdout (fd 1) і stderr (fd 2) */

int main(void)
{
    const char out[] =
        "Normal output\n";

    const char err[] =
        "Error output\n";

    write(
        STDOUT_FILENO,
        out,
        sizeof(out) - 1
    );

    write(
        STDERR_FILENO,
        err,
        sizeof(err) - 1
    );

    return 0;
}
