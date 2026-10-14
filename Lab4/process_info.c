#include <stdio.h>
#include <unistd.h>

/* Завдання 1-3: хто я для операційної системи? */

int main(void)
{
    printf("PID:  %d\n", getpid());
    printf("PPID: %d\n", getppid());

    sleep(30);

    return 0;
}
