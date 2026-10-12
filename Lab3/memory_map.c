#include <stdio.h>
#include <stdlib.h>

int global_initialized = 10;
int global_uninitialized;

static int static_initialized = 20;
static int static_uninitialized;

static void show_title(void)
{
    puts("Memory map experiment");
}

int main(void)
{
    const char *message = "Computer Systems";
    int local = 30;
    int *dynamic = malloc(sizeof(*dynamic));

    if (dynamic == NULL) {
        return 1;
    }
    *dynamic = 40;

    show_title();
    printf("Code/function:           %p\n", (void *)show_title);
    printf("String literal:          %p\n", (const void *)message);
    printf("Global initialized:      %p\n", (void *)&global_initialized);
    printf("Global uninitialized:    %p\n", (void *)&global_uninitialized);
    printf("Static initialized:      %p\n", (void *)&static_initialized);
    printf("Static uninitialized:    %p\n", (void *)&static_uninitialized);
    printf("Heap allocation:         %p\n", (void *)dynamic);
    printf("Stack/local:             %p\n", (void *)&local);

    free(dynamic);
    return 0;
}
