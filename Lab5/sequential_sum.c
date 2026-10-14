#include <stdio.h>
#include <stdlib.h>

/* Додаткове завдання: та сама сума в одному потоці, для порівняння часу.
   ./sequential_sum [N] */

int main(int argc, char *argv[])
{
    long n = 1000000;

    if (argc == 2) {
        n = atol(argv[1]);
    }

    long sum = 0;

    for (long i = 1; i <= n; i++) {
        sum += i;
    }

    long expected = n * (n + 1) / 2;

    printf("Sequential result: %ld\n", sum);
    printf("Expected result:   %ld\n", expected);
    printf("Correct: %s\n", sum == expected ? "YES" : "NO");

    return sum == expected ? EXIT_SUCCESS : EXIT_FAILURE;
}
