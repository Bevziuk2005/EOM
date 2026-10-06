#include <stdio.h>

/* Завдання 4: символ у C - це маленьке число */

int main(void)
{
    char symbols[] = {'A', 'B', 'a', '0', '9'};
    int count = sizeof(symbols) / sizeof(symbols[0]);

    for (int i = 0; i < count; i++) {
        char c = symbols[i];

        printf("Character: %c\n", c);
        printf("Decimal:   %d\n", c);
        printf("Hex:       0x%X\n", (unsigned char)c);
        printf("\n");
    }

    /* Бонус: символ можна додавати як число */
    char c = 'A';
    printf("'A' + 1 = %c (код %d)\n", c + 1, c + 1);

    return 0;
}
