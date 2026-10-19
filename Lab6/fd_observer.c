#include <stdio.h>
#include <unistd.h>

/* Завдання 7: процес чекає, поки ми дивимось ls -l /proc/PID/fd */

int main(void)
{
    printf("PID: %d\n", getpid());
    fflush(stdout);

    sleep(60);

    return 0;
}
