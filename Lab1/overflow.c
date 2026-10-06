#include <stdio.h>
#include <stdint.h>

/* Завдання 6: що буде, якщо до uint8_t(255) додати 1.
   Дивимось тільки на беззнаковий тип. */

int main(void)
{
    uint8_t value = 255;

    printf("Before: %u\n", value);
    value++;
    printf("After value++: %u\n", value);

    printf("\n--- ще кілька перевірок ---\n");

    uint8_t a = 254;
    printf("254         -> %u\n", a);

    a = 255;
    printf("255         -> %u\n", a);

    a = 255;
    a = a + 1;
    printf("255 + 1     -> %u\n", a);

    a = 255;
    a = a + 2;
    printf("255 + 2     -> %u\n", a);

    return 0;
}
