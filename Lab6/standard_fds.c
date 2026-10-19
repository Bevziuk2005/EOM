#include <stdio.h>
#include <unistd.h>

/* Завдання 7: стандартні дескриптори */

int main(void)
{
    printf("STDIN  = %d\n", STDIN_FILENO);
    printf("STDOUT = %d\n", STDOUT_FILENO);
    printf("STDERR = %d\n", STDERR_FILENO);

    return 0;
}
