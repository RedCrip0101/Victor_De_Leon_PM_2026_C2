#include <stdio.h>
#include <stdlib.h>

/* Archivos y caracteres.
El programa escribe caracteres en un archivo. */

void main(void)
{
    char p1;
    FILE *ar;
    ar = fopen("c:\\temp\\archivo.txt", "w");
    if (ar != NULL)
    {
        printf("Escribe tu frase")
        while ((p1=getchar()) != '\n')
            fputc(p1, ar);
        fclose(ar);


}
  else
    printf("No se puede abrir el archivo");
}
