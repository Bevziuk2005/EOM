#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

/* Фінальне завдання: сума 1 + 2 + ... + N на чотирьох threads.
   ./parallel_sum        -> N = 1 000 000
   ./parallel_sum 100000000   -> свій N

   Кожен thread рахує свою частину у ЛОКАЛЬНІЙ змінній sum,
   тому mutex у циклі не потрібен. Main збирає результати
   лише після pthread_join(). */

#define THREADS 4

typedef struct {
    long start;
    long end;
    long result;
} Task;

void *calculate_sum(void *arg)
{
    Task *task = arg;

    long sum = 0;

    for (long i = task->start;
         i <= task->end;
         i++) {

        sum += i;
    }

    task->result = sum;

    return NULL;
}

int main(int argc, char *argv[])
{
    long n = 1000000;

    if (argc == 2) {
        n = atol(argv[1]);
    }

    if (n < THREADS) {
        fprintf(stderr, "N must be at least %d\n", THREADS);
        return EXIT_FAILURE;
    }

    Task tasks[THREADS];
    pthread_t threads[THREADS];

    long chunk = n / THREADS;

    for (int i = 0; i < THREADS; i++) {
        tasks[i].start = i * chunk + 1;
        /* останній thread забирає залишок, якщо N не ділиться на 4 */
        tasks[i].end = (i == THREADS - 1) ? n : (i + 1) * chunk;
        tasks[i].result = 0;

        int rc = pthread_create(
            &threads[i],
            NULL,
            calculate_sum,
            &tasks[i]
        );

        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %d\n", rc);
            return EXIT_FAILURE;
        }
    }

    long total = 0;

    for (int i = 0; i < THREADS; i++) {
        pthread_join(threads[i], NULL);

        /* після join результат thread-а гарантовано готовий */
        total += tasks[i].result;
    }

    for (int i = 0; i < THREADS; i++) {
        printf("Thread %d: %ld ... %ld = %ld\n",
               i + 1,
               tasks[i].start,
               tasks[i].end,
               tasks[i].result);
    }

    long expected = n * (n + 1) / 2;

    printf("\nParallel result: %ld\n", total);
    printf("Expected result: %ld\n\n", expected);
    printf("Correct: %s\n", total == expected ? "YES" : "NO");

    return total == expected ? EXIT_SUCCESS : EXIT_FAILURE;
}
