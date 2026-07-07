#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Alumnos desde JSON.
El programa lee un archivo JSON que contiene una cantidad n de alumnos.
Primero determina cuantos elementos hay en el archivo, despues reserva
memoria dinamica del tamano exacto para guardarlos y finalmente los
imprime en pantalla. */

#define TAM_CAMPO 50

struct alumno
{
    char nombre[TAM_CAMPO];
    char apellido[TAM_CAMPO];
    float promedio;
    char materia[TAM_CAMPO];
};

char *LeeArchivo(const char *);
int CuentaAlumnos(const char *);
char *ExtraeTexto(char *, const char *, char *);
char *ExtraeNumero(char *, const char *, float *);
void Imprime(struct alumno *, int);

void main(void)
{
    char *buffer, *cursor;
    struct alumno *lista;
    int n, i;

    buffer = LeeArchivo("alumnos.json");
    if (buffer == NULL)
        return;

    /* Primero se cuenta la cantidad de elementos */
    n = CuentaAlumnos(buffer);
    printf("\nCantidad de alumnos encontrados en el JSON: %d\n", n);

    /* Se reserva la memoria dinamica exacta para esa cantidad */
    lista = (struct alumno *)malloc(n * sizeof(struct alumno));
    if (lista == NULL)
    {
        printf("\nNo hay memoria suficiente para el arreglo");
        free(buffer);
        return;
    }

    /* Se recorre de nuevo el archivo, ya cargado en memoria, para llenar cada elemento */
    cursor = buffer;
    for (i = 0; i < n; i++)
    {
        cursor = strstr(cursor, "\"nombre\"");

        cursor = ExtraeTexto(cursor, "nombre", lista[i].nombre);
        cursor = ExtraeTexto(cursor, "apellido", lista[i].apellido);
        cursor = ExtraeNumero(cursor, "promedio", &lista[i].promedio);
        cursor = ExtraeTexto(cursor, "materia", lista[i].materia);
    }

    Imprime(lista, n);

    free(lista);
    free(buffer);
}

/* Carga el contenido completo del archivo en un buffer de memoria */
char *LeeArchivo(const char *rutaArchivo)
{
    FILE *archivo;
    char *buffer;
    long tamano;
    size_t leidos;

    archivo = fopen(rutaArchivo, "r");
    if (archivo == NULL)
    {
        printf("\nNo se pudo abrir el archivo %s", rutaArchivo);
        return NULL;
    }

    fseek(archivo, 0, SEEK_END);
    tamano = ftell(archivo);
    rewind(archivo);

    buffer = (char *)malloc((tamano + 1) * sizeof(char));
    if (buffer == NULL)
    {
        printf("\nNo hay memoria suficiente para leer el archivo");
        fclose(archivo);
        return NULL;
    }

    leidos = fread(buffer, sizeof(char), tamano, archivo);
    buffer[leidos] = '\0';

    fclose(archivo);
    return buffer;
}

/* Cuenta cuantas veces aparece la clave "nombre", una por cada alumno */
int CuentaAlumnos(const char *buffer)
{
    int contador = 0;
    const char *pos = buffer;

    while ((pos = strstr(pos, "\"nombre\"")) != NULL)
    {
        contador++;
        pos += strlen("\"nombre\"");
    }
    return contador;
}

/* Extrae el valor de tipo texto ("clave": "valor") a partir de un
puntero. Devuelve un puntero a la posicion justo despues del valor */
char *ExtraeTexto(char *inicio, const char *clave, char *destino)
{
    char busqueda[30];
    char *p, *fin;
    int longitud;

    sprintf(busqueda, "\"%s\"", clave);

    p = strstr(inicio, busqueda);
    p = strchr(p, ':') + 1;
    while (*p == ' ')
        p++;
    p++;

    fin = strchr(p, '"');
    longitud = (int)(fin - p);

    strncpy(destino, p, longitud);
    destino[longitud] = '\0';

    return fin + 1;
}

/* Extrae el valor numerico ("clave": numero) a partir de un puntero */
char *ExtraeNumero(char *inicio, const char *clave, float *destino)
{
    char busqueda[30];
    char *p;

    sprintf(busqueda, "\"%s\"", clave);

    p = strstr(inicio, busqueda);
    p = strchr(p, ':') + 1;

    *destino = strtof(p, &p);
    return p;
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
