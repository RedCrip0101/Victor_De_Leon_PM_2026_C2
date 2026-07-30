#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Identificador de archivos.
   El programa, dado el nombre de un archivo, revisa su extension y
   compara el contenido real del archivo contra la firma esperada para
   esa extension. De esta forma determina si el contenido es valido
   o si el archivo esta mal nombrado / corrupto / disfrazado. */

#define TAM_NOMBRE 260
#define TAM_FIRMA 8

typedef struct
{
    char extension[10];
    unsigned char firma[TAM_FIRMA];
    int tamanoFirma;
} tipoArchivo;


tipoArchivo TIPOS[] =
{
    {".png",  {0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A}, 8},
    {".jpg",  {0xFF, 0xD8, 0xFF},                            3},
    {".jpeg", {0xFF, 0xD8, 0xFF},                            3},
    {".bmp",  {'B', 'M'},                                    2},
    {".pdf",  {'%', 'P', 'D', 'F'},                          4},
    {".zip",  {'P', 'K', 0x03, 0x04},                        4},
    {".gif",  {'G', 'I', 'F', '8'},                          4}
};

#define NUM_TIPOS (sizeof(TIPOS) / sizeof(tipoArchivo))

const char *ObtenerExtension(const char *NOMBRE);
int BuscarTipo(const char *EXT);
int LeerFirmaArchivo(const char *NOMBRE, unsigned char *BUFFER, int TAM);
int ValidarArchivo(const char *NOMBRE);

void main(void)
{
    char NOMBRE[TAM_NOMBRE];
    int RES;

    printf("\n===== IDENTIFICADOR DE ARCHIVOS =====");
    printf("\nIngrese el nombre del archivo a verificar: ");
    scanf("%s", NOMBRE);

    RES = ValidarArchivo(NOMBRE);

    if (RES == 1)
        printf("\nEl archivo %s es valido: su contenido coincide con la extension.", NOMBRE);
    else if (RES == 0)
        printf("\nEl archivo %s NO es valido: el contenido no coincide con la extension.", NOMBRE);
    else if (RES == -1)
        printf("\nNo se pudo abrir el archivo %s", NOMBRE);
    else
        printf("\nLa extension del archivo %s no esta registrada para verificar.", NOMBRE);
}

const char *ObtenerExtension(const char *NOMBRE)
{
    const char *EXT;

    EXT = strrchr(NOMBRE, '.');
    if (EXT == NULL)
        return "";

    return EXT;
}

int BuscarTipo(const char *EXT)
{
    int I;

    for (I = 0; I < NUM_TIPOS; I++)
        if (strcasecmp(EXT, TIPOS[I].extension) == 0)
            return I;

    return -1;
}

int LeerFirmaArchivo(const char *NOMBRE, unsigned char *BUFFER, int TAM)
{
    FILE *ARCHIVO;
    int LEIDOS;

    ARCHIVO = fopen(NOMBRE, "rb");
    if (ARCHIVO == NULL)
        return 0;

    LEIDOS = fread(BUFFER, 1, TAM, ARCHIVO);
    fclose(ARCHIVO);

    return (LEIDOS == TAM);
}

int ValidarArchivo(const char *NOMBRE)
{
    const char *EXT;
    int INDICE, I;
    unsigned char BUFFER[TAM_FIRMA];

    EXT = ObtenerExtension(NOMBRE);
    INDICE = BuscarTipo(EXT);

    if (INDICE == -1)
        return -2;

    if (!LeerFirmaArchivo(NOMBRE, BUFFER, TIPOS[INDICE].tamanoFirma))
        return -1;

    for (I = 0; I < TIPOS[INDICE].tamanoFirma; I++)
        if (BUFFER[I] != TIPOS[INDICE].firma[I])
            return 0;

    return 1;
}
