#include <stdio.h>

/* Завдання 1: скільки байтів займають основні типи */

int main(void)
{
    printf("%-12s %s %s\n", "Type", "bytes", "bits");
    printf("%-12s %5zu %4zu\n", "char",      sizeof(char),      sizeof(char) * 8);
    printf("%-12s %5zu %4zu\n", "short",     sizeof(short),     sizeof(short) * 8);
    printf("%-12s %5zu %4zu\n", "int",       sizeof(int),       sizeof(int) * 8);
    printf("%-12s %5zu %4zu\n", "long",      sizeof(long),      sizeof(long) * 8);
    printf("%-12s %5zu %4zu\n", "long long", sizeof(long long), sizeof(long long) * 8);
    printf("%-12s %5zu %4zu\n", "float",     sizeof(float),     sizeof(float) * 8);
    printf("%-12s %5zu %4zu\n", "double",    sizeof(double),    sizeof(double) * 8);
    printf("%-12s %5zu %4zu\n", "void *",    sizeof(void *),    sizeof(void *) * 8);

    return 0;
}
