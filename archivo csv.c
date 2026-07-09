#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Alumnos desde CSV.
El programa lee un archivo CSV que contiene una cantidad n de alumnos
(la primera linea es el encabezado). Primero determina cuantas lineas
de datos hay, despues reserva memoria dinamica del tamano exacto para
guardarlos y finalmente los imprime en pantalla. */

#define TAM_CAMPO 50
#define TAM_LINEA 200

struct alumno
{
    char nombre[TAM_CAMPO];
    char apellido[TAM_CAMPO];
    float promedio;
    char materia[TAM_CAMPO];
};

int CuentaAlumnos(FILE *);
void Imprime(struct alumno *, int);

void main(void)
{
    FILE *archivo;
    struct alumno *lista;
    char linea[TAM_LINEA], *token;
    int n, i;

    archivo = fopen("alumnos.csv", "r");
    if (archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo alumnos.csv");
        return;
    }

    /* Primero se cuenta la cantidad de elementos (sin el encabezado) */
    n = CuentaAlumnos(archivo);
    printf("\nCantidad de alumnos encontrados en el CSV: %d\n", n);

    /* Se reserva la memoria dinamica exacta para esa cantidad */
    lista = (struct alumno *)malloc(n * sizeof(struct alumno));
    if (lista == NULL)
    {
        printf("\nNo hay memoria suficiente para el arreglo");
        fclose(archivo);
        return;
    }

    /* Se vuelve al inicio del archivo para leer los datos reales */
    rewind(archivo);
    fgets(linea, TAM_LINEA, archivo);  /* se descarta el encabezado */

    for (i = 0; i < n; i++)
    {
        fgets(linea, TAM_LINEA, archivo);

        token = strtok(linea, ",");
        strcpy(lista[i].nombre, token);

        token = strtok(NULL, ",");
        strcpy(lista[i].apellido, token);

        token = strtok(NULL, ",");
        lista[i].promedio = atof(token);

        token = strtok(NULL, ",\n");
        strcpy(lista[i].materia, token);
    }

    fclose(archivo);

    Imprime(lista, n);

    free(lista);
}

/* Cuenta cuantas lineas de datos hay en el archivo (sin el encabezado) */
int CuentaAlumnos(FILE *archivo)
{
    char linea[TAM_LINEA];
    int contador = 0;

    fgets(linea, TAM_LINEA, archivo);  /* se descarta el encabezado */

    while (fgets(linea, TAM_LINEA, archivo) != NULL)
        if (strlen(linea) > 1)
            contador++;

    return contador;
}

/* Imprime el arreglo de alumnos ya cargado en memoria */
void Imprime(struct alumno *lista, int n)
{
    int i;

    printf("\n%-12s %-15s %-10s %-20s\n", "Nombre", "Apellido", "Promedio", "Materia");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < n; i++)
        printf("%-12s %-15s %-10.1f %-20s\n",
               lista[i].nombre, lista[i].apellido, lista[i].promedio, lista[i].materia);
}
