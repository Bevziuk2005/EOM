#include <stdio.h>

/* Завдання 2: одне й те саме число в decimal, hex і binary.
   Двійкову колонку виводимо тільки для того, щоб звірити
   свої ручні розрахунки. */

static void print_bits(unsigned int value)
{
    int started = 0;

    for (int i = 31; i >= 0; i--) {
        unsigned int bit = (value >> i) & 1u;

        if (bit) {
            started = 1;
        }
        if (started) {
            printf("%u", bit);
        }
    }
    if (!started) {
        printf("0");
    }
}

int main(void)
{
    unsigned int numbers[] = {10, 65, 127, 255, 256, 1024};
    int count = sizeof(numbers) / sizeof(numbers[0]);

    for (int i = 0; i < count; i++) {
        unsigned int value = numbers[i];

        printf("Decimal:     %u\n", value);
        printf("Hexadecimal: 0x%X\n", value);
        printf("Binary:      ");
        print_bits(value);
        printf("\n\n");
    }

    return 0;
}
