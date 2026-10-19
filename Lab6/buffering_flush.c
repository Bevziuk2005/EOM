#include <stdio.h>
#include <unistd.h>

/* Завдання 16: те саме, але з fflush(stdout) перед write() */

int main(void)
{
    printf("A");
    printf("B");
    printf("C");

    fflush(stdout);

    write(
        STDOUT_FILENO,
        "D",
        1
    );

    return 0;
}
