#include <stdio.h>
#include <stdlib.h>

/* Torre de Hanoi.
El programa resuelve el problema de la Torre de Hanoi para n discos de
forma recursiva. Cada movimiento se muestra en pantalla y ademas se
graba en un archivo de texto (movimientos.txt), una linea por
movimiento, junto con el numero de movimiento que le corresponde. */

void Hanoi(int, char, char, char, FILE *, int *);

void main(void)
{
    int discos, contador;
    FILE *archivo;

    contador = 0;

    do
    {
        printf("Cuantos discos quiere mover: ");
        scanf("%d", &discos);
        if (discos <= 0)
            printf("Error, ingrese un numero mayor a 0\n");
    }
    while (discos <= 0);

    archivo = fopen("movimientos.txt", "w");
    if (archivo == NULL)
    {
        printf("\nNo se pudo crear el archivo movimientos.txt");
        return;
    }

    printf("\nMovimientos\n");
    Hanoi(discos, 'A', 'C', 'B', archivo, &contador);

    fclose(archivo);

    printf("\nTotal de movimientos: %d\n", contador);
    printf("Los movimientos se guardaron en movimientos.txt\n");
}


void Hanoi(int n, char origen, char destino, char auxiliar, FILE *archivo, int *contador)
{
    if (n == 1)
    {
        (*contador)++;
        printf("Movimiento %d: mover disco 1 de %c a %c\n", *contador, origen, destino);
        fprintf(archivo, "Movimiento %d: mover disco 1 de %c a %c\n", *contador, origen, destino);
        return;
    }

    Hanoi(n - 1, origen, auxiliar, destino, archivo, contador);

    (*contador)++;
    printf("Movimiento %d: mover disco %d de %c a %c\n", *contador, n, origen, destino);
    fprintf(archivo, "Movimiento %d: mover disco %d de %c a %c\n", *contador, n, origen, destino);

    Hanoi(n - 1, auxiliar, destino, origen, archivo, contador);
}
