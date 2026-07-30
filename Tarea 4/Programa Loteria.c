#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* Loteria.
   El programa simula jugadas de loteria. El usuario elige un numero del 1 al 40
   y una cantidad de dinero. El programa genera un numero ganador al azar.
   Si el usuario acierta, el premio es la cantidad jugada multiplicada por 1000.*/

const int MAX = 100;

void RealizarJugadas(int *, float *, int);
void MostrarResultados(int *, float *, int);

void main(void)
{
    int OPC, NJUG = 0;
    int JUGADAS[MAX];
    float MONTOS[MAX];

    srand(time(NULL));    

    do
    {
        printf("\n===== MENU LOTERIA =====");
        printf("\n1. Jugar");
        printf("\n2. Elegir cantidad de jugadas");
        printf("\n3. Salir");
        printf("\n\nElija una opcion: ");
        scanf("%d", &OPC);

        switch (OPC)
        {
            case 1:
                if (NJUG == 0)
                    printf("\nPrimero debe elegir la cantidad de jugadas (opcion 2).");
                else
                {
                    RealizarJugadas(JUGADAS, MONTOS, NJUG);
                    MostrarResultados(JUGADAS, MONTOS, NJUG);
                }
                break;

            case 2:
                do
                {
                    printf("\nIngrese la cantidad de jugadas (1 a %d): ", MAX);
                    scanf("%d", &NJUG);
                }
                while (NJUG < 1 || NJUG > MAX);
                printf("\nCantidad de jugadas establecida: %d", NJUG);
                break;

            case 3:
                printf("\nSaliendo del programa. Hasta luego!");
                break;

            default:
                printf("\nOpcion invalida. Intente de nuevo.");
                break;
        }
    }
    while (OPC != 3);
}

void RealizarJugadas(int JUG[], float MON[], int N)

{
    int I;

    printf("\n\n--- Ingreso de jugadas ---");
    for (I = 0; I < N; I++)
    {
        do
        {
            printf("\nJugada %d - Ingrese su numero (1 a 40): ", I + 1);
            scanf("%d", &JUG[I]);
        }
        while (JUG[I] < 1 || JUG[I] > 40);

        printf("Jugada %d - Ingrese el monto a jugar: $", I + 1);
        scanf("%f", &MON[I]);
    }
}

void MostrarResultados(int JUG[], float MON[], int N)

{
    int I, NGAN;
    float PREMIO;

    NGAN = (rand() % 40) + 1;    

    printf("\n\n--- RESULTADO ---");
    printf("\nNumero ganador: %d\n", NGAN);

    for (I = 0; I < N; I++)
    {
        printf("\nJugada %d: Numero %d | Monto: $%.2f", I + 1, JUG[I], MON[I]);
        if (JUG[I] == NGAN)
        {
            PREMIO = MON[I] * 1000;
            printf("  >>> GANASTE! Premio: $%.2f <<<", PREMIO);
        }
        else
            printf("  (No acerto)");
    }
    printf("\n");
}
