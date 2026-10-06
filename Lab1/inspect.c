#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/* Підсумкова програма.
   Частина 1: однобайтове значення в різних формах.
   Частина 2 (додаткова): 32-бітне значення, байти в пам'яті
   і визначення endianness. */

static void print_binary(unsigned char value)
{
    for (int i = 7; i >= 0; i--) {
        printf("%u", (value >> i) & 1u);
    }
}

/* Порядок байтів визначаємо експериментально:
   записуємо 1 в uint32_t і дивимось, який байт лежить за найменшою адресою */
static const char *detect_endianness(void)
{
    uint32_t probe = 1;
    unsigned char *first = (unsigned char *)&probe;

    if (first[0] == 1) {
        return "Little-endian";
    }
    return "Big-endian";
}

static void inspect_byte(void)
{
    int input;

    printf("Enter value (0-255): ");
    if (scanf("%d", &input) != 1 || input < 0 || input > 255) {
        printf("Потрібне число від 0 до 255\n");
        exit(1);
    }

    unsigned char value = (unsigned char)input;

    printf("\n");
    printf("Decimal:      %u\n", value);
    printf("Hexadecimal:  0x%02X\n", value);
    printf("Binary:       ");
    print_binary(value);
    printf("\n");

    /* друкуємо символ тільки якщо його видно на екрані (ASCII 32..126) */
    if (value >= 32 && value <= 126) {
        printf("Character:    %c\n", value);
    } else {
        printf("Character:    (не відображається)\n");
    }
}

static void inspect_word(void)
{
    unsigned int input;

    printf("\nValue (hex, наприклад 12345678): ");
    if (scanf("%x", &input) != 1) {
        printf("Не вдалося прочитати число\n");
        return;
    }

    uint32_t value = input;
    unsigned char *p = (unsigned char *)&value;

    printf("\nValue: 0x%08X\n\n", value);
    printf("Size: %zu bytes\n\n", sizeof(value));

    printf("Memory:\n");
    for (size_t i = 0; i < sizeof(value); i++) {
        printf("byte %zu: 0x%02X\n", i, p[i]);
    }

    printf("\nByte order: %s\n", detect_endianness());
}

int main(void)
{
    inspect_byte();
    inspect_word();

    return 0;
}
