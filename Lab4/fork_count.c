#include <stdio.h>
#include <unistd.h>

/* Завдання 15: скільки разів надрукується рядок після двох fork()?
   Для експерименту з трьома fork() достатньо додати третій виклик. */

int main(void)
{
    fork();
    fork();

    printf("PID=%d PPID=%d\n", getpid(), getppid());

    return 0;
}
