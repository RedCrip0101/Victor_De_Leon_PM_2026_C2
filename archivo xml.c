#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Alumnos desde XML.
El programa lee un archivo XML que contiene una cantidad n de alumnos.
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
char *ExtraeCampo(char *, const char *, char *);
void Imprime(struct alumno *, int);

void main(void)
{
    char *buffer, campoTexto[TAM_CAMPO], *cursor;
    struct alumno *lista;
    int n, i;

    buffer = LeeArchivo("alumnos.xml");
    if (buffer == NULL)
        return;

    
    n = CuentaAlumnos(buffer);
    printf("\nCantidad de alumnos encontrados en el XML: %d\n", n);

  
    lista = (struct alumno *)malloc(n * sizeof(struct alumno));
    if (lista == NULL)
    {
        printf("\nNo hay memoria suficiente para el arreglo");
        free(buffer);
        return;
    }

    cursor = buffer;
    for (i = 0; i < n; i++)
    {
        cursor = strstr(cursor, "<Alumno>");

        cursor = ExtraeCampo(cursor, "Nombre", lista[i].nombre);
        cursor = ExtraeCampo(cursor, "Apellido", lista[i].apellido);

        cursor = ExtraeCampo(cursor, "Promedio", campoTexto);
        lista[i].promedio = atof(campoTexto);

        cursor = ExtraeCampo(cursor, "Materia", lista[i].materia);
    }

    Imprime(lista, n);

    free(lista);
    free(buffer);
}

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

int CuentaAlumnos(const char *buffer)
{
    int contador = 0;
    const char *pos = buffer;

    while ((pos = strstr(pos, "<Alumno>")) != NULL)
    {
        contador++;
        pos += strlen("<Alumno>");
    }
    return contador;
}


char *ExtraeCampo(char *inicio, const char *etiqueta, char *destino)
{
    char etiquetaApertura[30], etiquetaCierre[30];
    char *pInicio, *pFin;
    int longitud;

    sprintf(etiquetaApertura, "<%s>", etiqueta);
    sprintf(etiquetaCierre, "</%s>", etiqueta);

    pInicio = strstr(inicio, etiquetaApertura);
    pFin = strstr(pInicio, etiquetaCierre);

    pInicio += strlen(etiquetaApertura);
    longitud = (int)(pFin - pInicio);

    strncpy(destino, pInicio, longitud);
    destino[longitud] = '\0';

    return pFin + strlen(etiquetaCierre);
}


void Imprime(struct alumno *lista, int n)
{
    int i;

    printf("\n%-12s %-15s %-10s %-20s\n", "Nombre", "Apellido", "Promedio", "Materia");
    printf("--------------------------------------------------------------\n");
    for (i = 0; i < n; i++)
        printf("%-12s %-15s %-10.1f %-20s\n",
               lista[i].nombre, lista[i].apellido, lista[i].promedio, lista[i].materia);
}
