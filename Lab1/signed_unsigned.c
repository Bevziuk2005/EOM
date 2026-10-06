#include <stdio.h>
#include <stdint.h>

/* Завдання 5: одні й ті самі 8 бітів - два різних значення */

static void print_bits(uint8_t value)
{
    for (int i = 7; i >= 0; i--) {
        printf("%u", (value >> i) & 1u);
    }
}

int main(void)
{
    int8_t  signed_value   = -1;
    uint8_t unsigned_value = 255;

    printf("int8_t  = %d\n", signed_value);
    printf("uint8_t = %u\n", unsigned_value);
    printf("\n");

    /* перетворюємо на uint8_t, щоб побачити "сирі" біти */
    printf("int8_t  (-1)  hex: 0x%02X  bits: ", (uint8_t)signed_value);
    print_bits((uint8_t)signed_value);
    printf("\n");

    printf("uint8_t (255) hex: 0x%02X  bits: ", unsigned_value);
    print_bits(unsigned_value);
    printf("\n");

    printf("\n");
    printf("Біти однакові? %s\n",
           ((uint8_t)signed_value == unsigned_value) ? "так" : "ні");

    return 0;
}
