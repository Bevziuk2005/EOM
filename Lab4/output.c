#include <stdio.h>
#include <unistd.h>

/* Завдання 5: printf (бібліотека) проти write (системний виклик) */

int main(void)
{
    printf("Output using printf\n");

    const char message[] = "Output using write\n";

    write(STDOUT_FILENO,
          message,
          sizeof(message) - 1);

    return 0;
}
