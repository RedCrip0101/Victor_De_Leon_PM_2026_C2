#include <stdio.h>
#include <string.h>

/* Funciones de la biblioteca string.h para el manejo de cadenas de caracteres. */

void main(void)
{
    char *cad0 = "Hola Mexico";
    char cad1[20], cad2[20], cad3[20] = ", buenos dias!!!";

    strcpy(cad1, cad0);
    printf("\nPrueba de la función strcpy. Se copia la cadena cad0 a cad1: %s\n", cad1);

    strcpy(cad1, cad3);
    printf("\nPrueba de la función strcpy. Se copia la cadena cad3 a cad1: %s\n", cad1);

    strcpy(cad1, "XX");
    printf("\nPrueba de la funcion strcpy. Se cpía la cadena XX a cad 1: %s\n", cad1);

    strcpy(cad2, cad0, 4);
    cad2[4] = '\0';
    printf("\nPrueba de la función strncpy. Se copian 4 caracteres de cad0 a cad2: %s\n", cad2);

    strcpy(cad2, cad3, 3);
    cad2[3] = '\0';
    printf("\nPrueba de la función strncpy. Se copian 3 caracteres de cad3 a cad2: %s\n", cad2);

    strcat(cad0, cad3);
    printf("\nPrueba de la función strcat. Se incorpora la cadena cad3 a cad0: %s\n", cad0);

    strcat(cad1, " YY");
    strncat(cad2, cad0, 4);
    printf("\nPrueba de la función strncat. Se incorporan 4 caracteres de cad0 a cad2: %s\n", cad2);

    cad0 = strstr(cad0, "Mexico");
    printf("\nPrueba de la función strstr. Se trata de localizar la cadena México dentro de cad0: %s\n", cad0);

    cad0 = strstr(cad0, "Guateamala");
    printf("\nPrueba de la función strstr. Se trata de localizar la cadena Guatemala dentro de cad0: %s\n", cad0);
}


