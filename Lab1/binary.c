#include <stdio.h>

/* Завдання 3: виводимо 8 бітів байта.
   Починаємо зі старшого біта (7), щоб число читалося
   так само, як ми його записуємо на папері. */

void print_binary(unsigned char value)
{
    for (int i = 7; i >= 0; i--) {
        printf("%u", (value >> i) & 1u);
    }
}

int main(void)
{
    unsigned char tests[] = {0, 1, 10, 65, 127, 128, 255};
    int count = sizeof(tests) / sizeof(tests[0]);

    for (int i = 0; i < count; i++) {
        unsigned char value = tests[i];

        printf("Decimal: %u\n", value);
        printf("Hex:     0x%02X\n", value);
        printf("Binary:  ");
        print_binary(value);
        printf("\n\n");
    }

    return 0;
}
