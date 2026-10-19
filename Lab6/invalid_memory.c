#include <stdio.h>

/* Завдання 5: запис за нульовою адресою -> SIGSEGV */

int main(void)
{
    int *pointer = NULL;

    printf("About to access invalid memory\n");
    fflush(stdout);

    *pointer = 42;

    printf("You should not normally see this\n");

    return 0;
}
