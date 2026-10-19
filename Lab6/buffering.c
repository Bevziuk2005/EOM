#include <stdio.h>
#include <unistd.h>

/* Завдання 15: printf() іде через буфер stdio, write() - напряму в ядро */

int main(void)
{
    printf("A");
    printf("B");
    printf("C");

    write(
        STDOUT_FILENO,
        "D",
        1
    );

    return 0;
}
