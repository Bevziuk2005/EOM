#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Завдання 13: thread повертає результат через heap.
   Повертати адресу локальної змінної не можна: stack thread-а
   зникає разом із ним. */

void *calculate(void *arg)
{
    int value = *(int *)arg;

    int *result = malloc(sizeof(*result));

    if (result == NULL) {
        return NULL;
    }

    *result = value * value;

    return result;
}

int main(void)
{
    pthread_t thread;

    int value = 12;

    pthread_create(
        &thread,
        NULL,
        calculate,
        &value
    );

    void *thread_result = NULL;

    pthread_join(
        thread,
        &thread_result
    );

    if (thread_result != NULL) {
        int *result = thread_result;

        printf("Result = %d\n", *result);

        free(result);
    }

    return EXIT_SUCCESS;
}
