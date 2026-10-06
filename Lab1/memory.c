#include <stdio.h>
#include <stdint.h>

/* Завдання 7 і 8: дивимось на байти числа в пам'яті
   разом з їхніми адресами */

int main(void)
{
    uint32_t value = 0x12345678;

    unsigned char *p = (unsigned char *)&value;

    printf("value = 0x%08X\n\n", value);

    for (int i = 0; i < 4; i++) {
        printf("byte %d = 0x%02X\n", i, p[i]);
    }

    printf("\n%-20s %s\n", "Address", "Value");
    for (int i = 0; i < 4; i++) {
        printf("%-20p 0x%02X\n", (void *)&p[i], p[i]);
    }

    return 0;
}
