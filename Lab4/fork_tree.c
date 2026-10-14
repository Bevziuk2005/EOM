#include <stdio.h>
#include <unistd.h>

/* Завдання 15 (наочна версія fork_count.c).
   Усі процеси спочатку чекають секунду, друкують і ще секунду
   не завершуються. Так батьки ще живі, коли діти питають getppid(),
   і PPID показує справжнє дерево, а не "PPID=1" (процес-сирота). */

int main(void)
{
    fork();
    fork();

    sleep(1);
    printf("PID=%d PPID=%d\n", getpid(), getppid());
    fflush(stdout);

    sleep(1);

    return 0;
}
