#include <stdio.h>
#include <ctype.h>

/* Letras minúsculas y mayúsculas.
El programa determina el número de letras minúsculas y mayúsculas que hay en el archivo. */

void minymay(FILE *);

int main(void)
{
    FILE *ap;

    if ((ap = fopen("arc.txt", "r")) != NULL)
    {
        minymay(ap);
        fclose(ap);
    }
    else
    {
        printf("No se puede abrir el archivo\n");
    }

    return 0;
}


void minymay(FILE *ap1)
{
    char cad[30];
    int i, mi = 0, ma = 0;

    while (fgets(cad, 30, ap1) != NULL)
    {
        i = 0;
        while (cad[i] != '\0')
        {
            if (islower(cad[i]))
            {
                mi++;
            }
            else if (isupper(cad[i]))
            {
                ma++;
            }
            i++;
        }
    }

    printf("\nNumero de letras minusculas: %d", mi);
    printf("\nNumero de letras mayusculas: %d\n", ma);
}


