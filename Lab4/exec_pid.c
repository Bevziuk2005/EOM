#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* Завдання 10 (перевірка головної властивості exec):
   запускаємо new_program замість echo і порівнюємо PID до та після */

int main(void)
{
    printf("Before exec PID = %d\n", getpid());
    fflush(stdout);

    execl("./new_program", "new_program", NULL);

    perror("execl");

    return EXIT_FAILURE;
}
