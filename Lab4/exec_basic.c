#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Завдання 10: exec() замінює програму в тому самому процесі */

int main(void)
{
    printf("Before exec: PID=%d\n", getpid());
    fflush(stdout);

    execlp("echo",
           "echo",
           "Hello from exec!",
           NULL);

    /* сюди дійдемо тільки якщо exec не вдався */
    perror("execlp");

    return EXIT_FAILURE;
}
