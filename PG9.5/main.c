#include <stdio.h>
#include <stdlib.h>

/* Archivos con variables enteras y reales.
El programa almacena datos de un grupo de alumnos en un archivo. */

void main(void)
{
    int i, j, n, mat;
    float cal;
    FILE *ar;
    printf("\nIngrese el numero de alumnos: ");
    scanf("%d", &n);
    if ((ar = fopen("arc8.txt", "w")) != NULL)
    {
        fprintf(ar,"%d ", n);

        for (i=0; i<n; i++)
        {
         printf("\nIngrese la matrícula del alumno %d: ", i+1);
         scanf("%d", &mat);
         fprintf(ar,"\n%d ", mat);
         for (j=0; j<5; j++)
         {
             printf("\nCalificación %d: ", j+1);
             scanf("%f", &cal);
             fprintf(ar,"%.2f ", cal);
         }
        }
        fclose(ar);
    }
    else
        printf("No se puede abrir el archivo");
}
