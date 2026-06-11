#include <stdio.h>
#include <stdlib.h>

/* Funciones y parámetros. */

int a, b, c, d;

int pal(int, int);

void main(void)
{
    a = 2;
    b = 3;
    c = 5;
    a = pal(c, d);
    printf("\n%d %d %d %d", a, b, c, d);
    b = 4;
    b = pal(b, a);
    printf("\n%d %d %d %d", a, b, c, d);
}

int pal(int x, int y)
{
    int c;
    b = x * y;
    c = b + y;
    x++;
    y = y * (y + 1);
    printf("\n%d %d %d %d", b, c, x, y);
    return (x);


}
