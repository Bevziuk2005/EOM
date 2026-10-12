#include <stdio.h>

/* Бере два числа, визначає більше й менше,
   а потім у циклі "розкладає" менше число на більше:
   складає більше число стільки разів, скільки одиниць у меншому.
   Фактично це множення через додавання. */

int mul_by_add(int a, int b)
{
    int big;
    int small;
    int total = 0;

    if (a > b) {
        big = a;
        small = b;
    } else {
        big = b;
        small = a;
    }

    for (int i = 0; i < small; i++) {
        total = total + big;
    }

    return total;
}

int main(void)
{
    int answer = mul_by_add(6, 4);

    printf("6 * 4 = %d\n", answer);

    return 0;
}
