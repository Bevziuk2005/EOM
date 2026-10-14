#include <stdio.h>
#include <unistd.h>

/* Допоміжна програма для перевірки: чи зберігається PID після exec().
   Використовується разом з exec_pid.c */

int main(void)
{
    printf("New program PID=%d\n", getpid());
    return 0;
}
