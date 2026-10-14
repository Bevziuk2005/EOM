#include <stdio.h>
#include <unistd.h>

/* Завдання 13: процес, який просто спить. Дивимось на його State */

int main(void)
{
    printf("PID=%d\n", getpid());
    fflush(stdout);

    sleep(60);

    return 0;
}
