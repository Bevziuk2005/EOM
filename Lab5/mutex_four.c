#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Завдання 11: чотири threads по 500000 increments, З mutex */

#define THREADS    4
#define ITERATIONS 500000

long counter = 0;

pthread_mutex_t mutex =
    PTHREAD_MUTEX_INITIALIZER;

void *increment(void *arg)
{
    (void)arg;

    for (long i = 0; i < ITERATIONS; i++) {
        pthread_mutex_lock(&mutex);
        counter++;
        pthread_mutex_unlock(&mutex);
    }

    return NULL;
}

int main(void)
{
    pthread_t threads[THREADS];

    for (int i = 0; i < THREADS; i++) {
        pthread_create(&threads[i], NULL, increment, NULL);
    }

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    printf("Expected: %ld\n",
           (long)THREADS * ITERATIONS);

    printf("Actual:   %ld\n",
           counter);

    pthread_mutex_destroy(&mutex);

    return EXIT_SUCCESS;
}
