#include <stdio.h>
#include <stdlib.h>

void trueque(int *x, int *y)
{
    int tem;
    tem = *x;
    *x = *y;
    *y = tem;
}

int suma(int x)
{
    return (x + x);
}

int main(void)
{
    int a = 3, b = 7;

    printf("Antes del trueque: a=%d, b=%d\n", a, b);
    trueque(&a, &b);
    printf("Despues del trueque: a=%d, b=%d\n", a, b);

    printf("suma (5) = %d\n", suma(5));

    return (0);
}
