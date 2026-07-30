#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

/* Conversor de imagenes a blanco y negro.
   El programa hace una lista de los archivos png, jpg y bmp que se
   encuentran en la carpeta actual, el usuario elige uno desde el
   menu y el programa lo convierte a escala de grises generando un
   nuevo archivo con el prefijo "BN_".*/

#define MAX_ARCHIVOS 100
#define TAM_NOMBRE 260

typedef struct
{
    char nombre[TAM_NOMBRE];
} archivoImagen;

int ListarImagenes(archivoImagen LISTA[], int MAX);
void MostrarMenu(archivoImagen LISTA[], int N);
int EsExtensionValida(const char *NOMBRE);
void ConvertirABN(const char *NOMBRE);

void main(void)
{
    archivoImagen LISTA[MAX_ARCHIVOS];
    int N, OPC;

    N = ListarImagenes(LISTA, MAX_ARCHIVOS);

    if (N == 0)
    {
        printf("\nNo se encontraron archivos png, jpg o bmp en la carpeta actual.");
        return;
    }

    do
    {
        MostrarMenu(LISTA, N);
        printf("\nElija el numero del archivo a convertir (0 para salir): ");
        scanf("%d", &OPC);

        if (OPC >= 1 && OPC <= N)
            ConvertirABN(LISTA[OPC - 1].nombre);
        else if (OPC != 0)
            printf("\nOpcion invalida. Intente de nuevo.");
    }
    while (OPC != 0);

    printf("\nSaliendo del programa. Hasta luego!");
}

int ListarImagenes(archivoImagen LISTA[], int MAX)
{
    DIR *DIRECTORIO;
    struct dirent *ENTRADA;
    int N = 0;

    DIRECTORIO = opendir(".");
    if (DIRECTORIO == NULL)
    {
        printf("\nNo se pudo abrir la carpeta actual.");
        return 0;
    }

    while ((ENTRADA = readdir(DIRECTORIO)) != NULL && N < MAX)
    {
        if (EsExtensionValida(ENTRADA->d_name))
        {
            strcpy(LISTA[N].nombre, ENTRADA->d_name);
            N++;
        }
    }

    closedir(DIRECTORIO);
    return N;
}

int EsExtensionValida(const char *NOMBRE)
{
    const char *EXT;

    EXT = strrchr(NOMBRE, '.');
    if (EXT == NULL)
        return 0;

    if (strcasecmp(EXT, ".png") == 0)
        return 1;
    if (strcasecmp(EXT, ".jpg") == 0)
        return 1;
    if (strcasecmp(EXT, ".jpeg") == 0)
        return 1;
    if (strcasecmp(EXT, ".bmp") == 0)
        return 1;

    return 0;
}

void MostrarMenu(archivoImagen LISTA[], int N)
{
    int I;

    printf("\n\n===== MENU CONVERSOR A BLANCO Y NEGRO =====");
    for (I = 0; I < N; I++)
        printf("\n%d. %s", I + 1, LISTA[I].nombre);
    printf("\n0. Salir");
}

void ConvertirABN(const char *NOMBRE)
{
    unsigned char *DATOS, *SALIDA;
    int ANCHO, ALTO, CANALES;
    int I, PIXEL;
    unsigned char GRIS;
    char RUTA_SALIDA[TAM_NOMBRE + 3];
    const char *EXT;

    DATOS = stbi_load(NOMBRE, &ANCHO, &ALTO, &CANALES, 3);
    if (DATOS == NULL)
    {
        printf("\nNo se pudo abrir o leer la imagen %s", NOMBRE);
        return;
    }

    SALIDA = (unsigned char *)malloc(ANCHO * ALTO * 3);
    if (SALIDA == NULL)
    {
        printf("\nNo hay memoria suficiente para procesar la imagen.");
        stbi_image_free(DATOS);
        return;
    }

    for (I = 0; I < ANCHO * ALTO; I++)
    {
        PIXEL = I * 3;
        GRIS = (unsigned char)(0.299 * DATOS[PIXEL] + 0.587 * DATOS[PIXEL + 1] + 0.114 * DATOS[PIXEL + 2]);
        SALIDA[PIXEL] = GRIS;
        SALIDA[PIXEL + 1] = GRIS;
        SALIDA[PIXEL + 2] = GRIS;
    }

    sprintf(RUTA_SALIDA, "BN_%s", NOMBRE);
    EXT = strrchr(NOMBRE, '.');

    if (strcasecmp(EXT, ".png") == 0)
        stbi_write_png(RUTA_SALIDA, ANCHO, ALTO, 3, SALIDA, ANCHO * 3);
    else if (strcasecmp(EXT, ".bmp") == 0)
        stbi_write_bmp(RUTA_SALIDA, ANCHO, ALTO, 3, SALIDA);
    else
        stbi_write_jpg(RUTA_SALIDA, ANCHO, ALTO, 3, SALIDA, 90);

    printf("\nImagen convertida correctamente. Se genero el archivo: %s", RUTA_SALIDA);

    stbi_image_free(DATOS);
    free(SALIDA);
}
