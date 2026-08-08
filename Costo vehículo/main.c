/* ============================================================
   PROGRAMA: Calculadora de Costo Real de Vehiculos por Kilometro
   ------------------------------------------------------------

   OBJETIVO
   --------
   Calcular cuanto cuesta REALMENTE mover un vehiculo por kilometro,
   tomando en cuenta no solo el combustible, sino tambien la
   depreciacion (amortizacion), el mantenimiento, el seguro y el
   desgaste de los neumaticos. El programa permite registrar
   vehiculos, editarlos, borrarlos, simular viajes y actualizar el
   precio del combustible. Toda la informacion se guarda en archivos
   de texto para que persista entre ejecuciones.

   LOGICA DE CALCULO
   ----------------------------------------------------------------
   Costos FIJOS anuales (no dependen de cuanto manejes ese dia, pero
   se reparten entre los km que se recorren en el año):
       - Depreciacion anual   = costoInicial / vidaUtilAnios (años)
       - Mantenimiento anual  (dato de entrada)
       - Seguro anual         (dato de entrada)
       - Imprevistos anual    (dato de entrada: reparaciones no
         cubiertas por el seguro, multas, averias inesperadas, etc.
         Es un "colchon" presupuestario, comun en la administracion
         de flotas, para que un gasto inesperado no descuadre las
         cuentas del vehiculo)
   Estos cuatro se dividen entre kmAnualesCiudad + kmAnualesAutopista
   para obtener un costo fijo "por km".

   Costos VARIABLES por km (si dependen de cuanto manejas):
       - Neumaticos  = costoJuegoNeumaticos / vidaUtilNeumaticosKm
       - Combustible = precioCombustible / consumo(km por galon)
         (el consumo es distinto en ciudad y en autopista)

   Costo real por km (ciudad o autopista) =
       depreciacion/km + mantenimiento/km + seguro/km + imprevistos/km
       + neumaticos/km + combustible/km

   Al calcular un viaje, el programa muestra el DESGLOSE completo
   (cuanto corresponde a cada rubro), no solo el total, para poder
   explicar de donde sale cada peso del costo del viaje.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARCHIVO_VEHICULOS   "vehiculos.txt"
#define ARCHIVO_COMBUSTIBLE "combustible.txt"
#define MAX_NOMBRE 50

typedef struct {
    char  nombre[MAX_NOMBRE];        /* Identificador: marca/modelo/placa   */
    float costoInicial;              /* Costo de compra del vehiculo (RD$)  */
    int   vidaUtilAnios;             /* Años de vida útil (para depreciar)  */
    float gastoMantAnual;            /* Gasto de mantenimiento anual (RD$)  */
    float seguroAnual;               /* Costo del seguro anual (RD$)        */
    float costoJuegoNeumaticos;      /* Costo de un juego de 4 gomas (RD$)  */
    int   vidaUtilNeumaticosKm;      /* Km que dura un juego de gomas       */
    float consumoCiudad;             /* Km recorridos por litro en ciudad   */
    float consumoAutopista;          /* Km recorridos por litro en autopista*/
    float kmAnualesCiudad;           /* Km promedio recorridos/año, ciudad  */
    float kmAnualesAutopista;        /* Km promedio recorridos/año, aut.    */
    float imprevistosAnual;          /* Presupuesto anual para imprevistos  */
                                      /* (reparaciones no planificadas,     */
                                      /* multas, remolque, etc.)            */
} Vehiculo;

/* ------------------------------------------------------------
   Desglose de costo por km: guarda CADA componente por separado
   (en vez de solo el total) para poder explicar de donde sale
   cada peso cuando se calcula un viaje.
   ------------------------------------------------------------ */
typedef struct {
    float depreciacionPorKm;
    float mantenimientoPorKm;
    float seguroPorKm;
    float neumaticosPorKm;
    float imprevistosPorKm;
    float combustibleCiudadPorKm;
    float combustibleAutopistaPorKm;
    float realCiudadPorKm;       /* suma de todo lo anterior, ciudad    */
    float realAutopistaPorKm;    /* suma de todo lo anterior, autopista */
} DesgloseCosto;

/* ---------- Prototipos ---------- */
int   cargarVehiculos(Vehiculo **lista);
void  guardarVehiculos(Vehiculo *lista, int n);
void  crearVehiculo(void);
void  borrarVehiculo(void);
void  modificarVehiculo(void);
void  listarVehiculos(Vehiculo *lista, int n);
int   buscarVehiculo(Vehiculo *lista, int n, const char *nombre);
float leerPrecioCombustible(void);
void  guardarPrecioCombustible(float precio);
void  modificarPrecioCombustible(void);
void  calcularDesglose(Vehiculo v, float precioCombustible, DesgloseCosto *d);
void  calcularViaje(void);
void  limpiarBuffer(void);
void  pausar(void);
void  menuPrincipal(void);

/* ============================================================
   MAIN
   ============================================================ */
int main(void) {
    menuPrincipal();
    return 0;
}

/* ============================================================
   UTILIDADES DE ENTRADA
   ============================================================ */
void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descarta el resto de la linea */ }
}

void pausar(void) {
    /* En este punto el buffer ya deberia estar limpio (cada funcion de
       menu limpia su propio buffer al terminar), asi que basta con
       esperar el ENTER real del usuario sin volver a limpiar. */
    printf("\nPresione ENTER para continuar...");
    getchar();
}

int cargarVehiculos(Vehiculo **lista) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "r");
    int capacidad = 4, n = 0;
    Vehiculo *arr;

    if (f == NULL) {
        /* Aun no existe el archivo -> no hay vehiculos registrados */
        *lista = NULL;
        return 0;
    }

    arr = (Vehiculo *) malloc(capacidad * sizeof(Vehiculo));
    if (arr == NULL) {
        printf("Error: no hay memoria disponible.\n");
        fclose(f);
        exit(1);
    }

    /* El espacio inicial " " en cada formato salta blancos y lineas en
       blanco que puedan quedar entre un bloque y el siguiente. */
    while (fscanf(f, " Vehiculo: %49[^\n]", arr[n].nombre) == 1) {
        fscanf(f, " Costo: %f", &arr[n].costoInicial);
        fscanf(f, " VidaUtilAnios: %d", &arr[n].vidaUtilAnios);
        fscanf(f, " GastoMantAnual: %f", &arr[n].gastoMantAnual);
        fscanf(f, " SeguroAnual: %f", &arr[n].seguroAnual);
        fscanf(f, " CostoNeumaticos: %f", &arr[n].costoJuegoNeumaticos);
        fscanf(f, " VidaUtilNeumaticosKm: %d", &arr[n].vidaUtilNeumaticosKm);
        fscanf(f, " ConsumoCiudad: %f", &arr[n].consumoCiudad);
        fscanf(f, " ConsumoAutopista: %f", &arr[n].consumoAutopista);
        fscanf(f, " KmAnualesCiudad: %f", &arr[n].kmAnualesCiudad);
        fscanf(f, " KmAnualesAutopista: %f", &arr[n].kmAnualesAutopista);
        fscanf(f, " ImprevistosAnual: %f", &arr[n].imprevistosAnual);
        fscanf(f, " ---");

        n++;
        if (n >= capacidad) {
            capacidad *= 2;
            arr = (Vehiculo *) realloc(arr, capacidad * sizeof(Vehiculo));
            if (arr == NULL) {
                printf("Error: no hay memoria disponible.\n");
                fclose(f);
                exit(1);
            }
        }
    }

    fclose(f);
    *lista = arr;
    return n;
}

void guardarVehiculos(Vehiculo *lista, int n) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "w");
    int i;

    if (f == NULL) {
        printf("Error: no se pudo abrir %s para escribir.\n", ARCHIVO_VEHICULOS);
        return;
    }

    for (i = 0; i < n; i++) {
        fprintf(f, "Vehiculo: %s\n", lista[i].nombre);
        fprintf(f, "Costo: %.2f\n", lista[i].costoInicial);
        fprintf(f, "VidaUtilAnios: %d\n", lista[i].vidaUtilAnios);
        fprintf(f, "GastoMantAnual: %.2f\n", lista[i].gastoMantAnual);
        fprintf(f, "SeguroAnual: %.2f\n", lista[i].seguroAnual);
        fprintf(f, "CostoNeumaticos: %.2f\n", lista[i].costoJuegoNeumaticos);
        fprintf(f, "VidaUtilNeumaticosKm: %d\n", lista[i].vidaUtilNeumaticosKm);
        fprintf(f, "ConsumoCiudad: %.2f\n", lista[i].consumoCiudad);
        fprintf(f, "ConsumoAutopista: %.2f\n", lista[i].consumoAutopista);
        fprintf(f, "KmAnualesCiudad: %.2f\n", lista[i].kmAnualesCiudad);
        fprintf(f, "KmAnualesAutopista: %.2f\n", lista[i].kmAnualesAutopista);
        fprintf(f, "ImprevistosAnual: %.2f\n", lista[i].imprevistosAnual);
        fprintf(f, "---\n\n");
    }

    fclose(f);
}

int buscarVehiculo(Vehiculo *lista, int n, const char *nombre) {
    int i;
    for (i = 0; i < n; i++) {
        if (strcmp(lista[i].nombre, nombre) == 0) return i;
    }
    return -1;
}

void listarVehiculos(Vehiculo *lista, int n) {
    int i;
    if (n == 0) {
        printf("No hay vehiculos registrados todavia.\n");
        return;
    }
    printf("\n%-20s %12s %10s\n", "NOMBRE", "COSTO", "VIDA(años)");
    printf("----------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-20s %12.2f %10d\n",
               lista[i].nombre, lista[i].costoInicial, lista[i].vidaUtilAnios);
    }
}

/* ============================================================
   MANEJO DE ARCHIVO - PRECIO DEL COMBUSTIBLE
   ============================================================ */
float leerPrecioCombustible(void) {
    FILE *f = fopen(ARCHIVO_COMBUSTIBLE, "r");
    /* Valor por defecto si no existe el archivo: precio oficial de la
       gasolina premium en Republica Dominicana segun el MICM para la
       semana del 8 al 14 de agosto de 2026 (fuente: acento.com.do,
       7 de agosto de 2026). El usuario puede actualizarlo cuando
       cambie con la opcion 5 del menu. */
    float precio = 341.10f;

    if (f == NULL) {
        guardarPrecioCombustible(precio);
        return precio;
    }

    if (fscanf(f, "%f", &precio) != 1) {
        precio = 341.10f;
    }
    fclose(f);
    return precio;
}

void guardarPrecioCombustible(float precio) {
    FILE *f = fopen(ARCHIVO_COMBUSTIBLE, "w");
    if (f == NULL) {
        printf("Error: no se pudo guardar el precio del combustible.\n");
        return;
    }
    fprintf(f, "%.2f\n", precio);
    fclose(f);
}

void modificarPrecioCombustible(void) {
    float actual = leerPrecioCombustible();
    float nuevo;

    printf("\n--- MODIFICAR PRECIO DEL COMBUSTIBLE ---\n");
    printf("Precio actual: RD$ %.2f por galon\n", actual);
    printf("Nuevo precio: RD$ ");
    if (scanf("%f", &nuevo) != 1 || nuevo <= 0) {
        printf("Precio invalido. No se realizo ningun cambio.\n");
        limpiarBuffer();
        return;
    }
    limpiarBuffer();

    guardarPrecioCombustible(nuevo);
    printf("Precio del combustible actualizado a RD$ %.2f por galon.\n", nuevo);
}

/* ============================================================
   CREAR VEHICULO
   ============================================================ */
void crearVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);
    Vehiculo nuevo;

    printf("\n--- REGISTRAR NUEVO VEHICULO ---\n");

    printf("Nombre/identificador (marca-modelo-placa): ");
    limpiarBuffer();
    fgets(nuevo.nombre, MAX_NOMBRE, stdin);
    nuevo.nombre[strcspn(nuevo.nombre, "\n")] = '\0'; /* quita el salto de linea */

    if (buscarVehiculo(lista, n, nuevo.nombre) != -1) {
        printf("Ya existe un vehiculo con ese nombre. Operacion cancelada.\n");
        free(lista);
        return;
    }

    printf("Costo de compra (RD$): ");
    scanf("%f", &nuevo.costoInicial);
    printf("Vida útil (años): ");
    scanf("%d", &nuevo.vidaUtilAnios);
    printf("Gasto de mantenimiento ANUAL estimado (RD$): ");
    scanf("%f", &nuevo.gastoMantAnual);
    printf("Seguro ANUAL (RD$): ");
    scanf("%f", &nuevo.seguroAnual);
    printf("Costo de un juego de neumaticos (RD$): ");
    scanf("%f", &nuevo.costoJuegoNeumaticos);
    printf("Vida útil de los neumáticos (km): ");
    scanf("%d", &nuevo.vidaUtilNeumaticosKm);
    printf("Consumo en CIUDAD (km recorridos por galon): ");
    scanf("%f", &nuevo.consumoCiudad);
    printf("Consumo en AUTOPISTA (km recorridos por galon): ");
    scanf("%f", &nuevo.consumoAutopista);
    printf("Km promedio recorridos POR AÑO en ciudad: ");
    scanf("%f", &nuevo.kmAnualesCiudad);
    printf("Km promedio recorridos POR AÑO en autopista: ");
    scanf("%f", &nuevo.kmAnualesAutopista);
    printf("Presupuesto ANUAL para imprevistos (reparaciones\n");
    printf("  inesperadas, multas, remolque, etc.) (RD$): ");
    scanf("%f", &nuevo.imprevistosAnual);
    limpiarBuffer();

    /* Reasignamos el arreglo con espacio para el nuevo vehiculo */
    lista = (Vehiculo *) realloc(lista, (n + 1) * sizeof(Vehiculo));
    if (lista == NULL) {
        printf("Error: no hay memoria disponible.\n");
        return;
    }
    lista[n] = nuevo;
    n++;

    guardarVehiculos(lista, n);
    printf("\nVehiculo '%s' guardado correctamente en %s.\n",
           nuevo.nombre, ARCHIVO_VEHICULOS);

    free(lista);
}

/* ============================================================
   BORRAR VEHICULO
   ============================================================ */
void borrarVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);
    char nombre[MAX_NOMBRE];
    int idx, i;

    listarVehiculos(lista, n);
    if (n == 0) return;

    printf("\nNombre del vehiculo a borrar: ");
    limpiarBuffer();
    fgets(nombre, MAX_NOMBRE, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    idx = buscarVehiculo(lista, n, nombre);
    if (idx == -1) {
        printf("No se encontro un vehiculo con ese nombre.\n");
        free(lista);
        return;
    }

    /* Desplazamos los elementos siguientes una posicion hacia atras */
    for (i = idx; i < n - 1; i++) {
        lista[i] = lista[i + 1];
    }
    n--;

    guardarVehiculos(lista, n);
    printf("Vehiculo '%s' eliminado.\n", nombre);

    free(lista);
}

/* ============================================================
   MODIFICAR VEHICULO
   ============================================================ */
void modificarVehiculo(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);
    char nombre[MAX_NOMBRE];
    int idx, opcion;

    listarVehiculos(lista, n);
    if (n == 0) return;

    printf("\nNombre del vehiculo a modificar: ");
    limpiarBuffer();
    fgets(nombre, MAX_NOMBRE, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    idx = buscarVehiculo(lista, n, nombre);
    if (idx == -1) {
        printf("No se encontro un vehiculo con ese nombre.\n");
        free(lista);
        return;
    }

    do {
        printf("\n--- MODIFICAR '%s' ---\n", lista[idx].nombre);
        printf("1. Costo de compra (actual: %.2f)\n", lista[idx].costoInicial);
        printf("2. Vida útil en años (actual: %d)\n", lista[idx].vidaUtilAnios);
        printf("3. Mantenimiento anual (actual: %.2f)\n", lista[idx].gastoMantAnual);
        printf("4. Seguro anual (actual: %.2f)\n", lista[idx].seguroAnual);
        printf("5. Costo juego de neumaticos (actual: %.2f)\n", lista[idx].costoJuegoNeumaticos);
        printf("6. Vida útil neumáticos en km (actual: %d)\n", lista[idx].vidaUtilNeumaticosKm);
        printf("7. Consumo en ciudad, km/galon (actual: %.2f)\n", lista[idx].consumoCiudad);
        printf("8. Consumo en autopista, km/galon (actual: %.2f)\n", lista[idx].consumoAutopista);
        printf("9. Km anuales en ciudad (actual: %.2f)\n", lista[idx].kmAnualesCiudad);
        printf("10. Km anuales en autopista (actual: %.2f)\n", lista[idx].kmAnualesAutopista);
        printf("11. Presupuesto anual de imprevistos (actual: %.2f)\n", lista[idx].imprevistosAnual);
        printf("0. Terminar y guardar\n");
        printf("Opcion: ");
        scanf("%d", &opcion);

        switch (opcion) {
            case 1: printf("Nuevo costo: "); scanf("%f", &lista[idx].costoInicial); break;
            case 2: printf("Nueva vida útil (años): "); scanf("%d", &lista[idx].vidaUtilAnios); break;
            case 3: printf("Nuevo mantenimiento anual: "); scanf("%f", &lista[idx].gastoMantAnual); break;
            case 4: printf("Nuevo seguro anual: "); scanf("%f", &lista[idx].seguroAnual); break;
            case 5: printf("Nuevo costo de neumaticos: "); scanf("%f", &lista[idx].costoJuegoNeumaticos); break;
            case 6: printf("Nueva vida útil de neumáticos (km): "); scanf("%d", &lista[idx].vidaUtilNeumaticosKm); break;
            case 7: printf("Nuevo consumo en ciudad: "); scanf("%f", &lista[idx].consumoCiudad); break;
            case 8: printf("Nuevo consumo en autopista: "); scanf("%f", &lista[idx].consumoAutopista); break;
            case 9: printf("Nuevos km anuales en ciudad: "); scanf("%f", &lista[idx].kmAnualesCiudad); break;
            case 10: printf("Nuevos km anuales en autopista: "); scanf("%f", &lista[idx].kmAnualesAutopista); break;
            case 11: printf("Nuevo presupuesto anual de imprevistos: "); scanf("%f", &lista[idx].imprevistosAnual); break;
            case 0: break;
            default: printf("Opcion invalida.\n");
        }
        limpiarBuffer();
    } while (opcion != 0);

    guardarVehiculos(lista, n);
    printf("Cambios guardados.\n");

    free(lista);
}

/* ============================================================
   CALCULO DEL DESGLOSE DE COSTO POR KM (ciudad y autopista)
   ------------------------------------------------------------
   Recibe un vehiculo y el precio del combustible, y llena un
   DesgloseCosto con CADA componente por separado (no solo el
   total). Esto es lo que permite, mas adelante, explicar en el
   resultado del viaje "de donde sale" cada peso del costo, en
   vez de mostrar solo un numero final sin justificar.
   ============================================================ */
void calcularDesglose(Vehiculo v, float precioCombustible, DesgloseCosto *d) {
    float kmAnualesTotales = v.kmAnualesCiudad + v.kmAnualesAutopista;
    float costoFijoPorKm;

    if (kmAnualesTotales <= 0) kmAnualesTotales = 1; /* evita division entre 0 */

    /* --- 1. Amortizacion / depreciacion: el vehiculo pierde valor cada
       año independientemente de cuanto se use; se reparte entre los km
       que se recorren en promedio al año --- */
    d->depreciacionPorKm = (v.costoInicial / v.vidaUtilAnios) / kmAnualesTotales;

    /* --- 2. Mantenimiento y seguro: gastos fijos anuales, tambien se
       reparten entre el total de km recorridos al año --- */
    d->mantenimientoPorKm = v.gastoMantAnual / kmAnualesTotales;
    d->seguroPorKm        = v.seguroAnual    / kmAnualesTotales;

    /* --- 7. Neumaticos: a diferencia de los anteriores, este gasto se
       vincula directamente al USO (los kilometros que se manejan), no
       al tiempo. Por eso se divide entre su vida util EN KM y no entre
       los km anuales; cada km recorrido "consume" una fraccion del
       juego de neumaticos actual --- */
    d->neumaticosPorKm = v.costoJuegoNeumaticos / v.vidaUtilNeumaticosKm;

    /* --- Imprevistos: presupuesto anual para lo que no se puede
       predecir (una reparacion inesperada, una multa, un remolque).
       Se trata igual que el mantenimiento: es un gasto fijo del año
       que se reparte entre los km recorridos --- */
    d->imprevistosPorKm = v.imprevistosAnual / kmAnualesTotales;

    /* Suma de todos los costos fijos ya expresados por km */
    costoFijoPorKm = d->depreciacionPorKm + d->mantenimientoPorKm +
                     d->seguroPorKm + d->neumaticosPorKm + d->imprevistosPorKm;

    /* --- 3. Combustible por km (distinto en ciudad y autopista, porque
       el consumo del vehiculo cambia segun el tipo de via) --- */
    d->combustibleCiudadPorKm    = precioCombustible / v.consumoCiudad;
    d->combustibleAutopistaPorKm = precioCombustible / v.consumoAutopista;

    /* --- 5. Costo real por km = fijos + combustible --- */
    d->realCiudadPorKm    = costoFijoPorKm + d->combustibleCiudadPorKm;
    d->realAutopistaPorKm = costoFijoPorKm + d->combustibleAutopistaPorKm;
}

/* ============================================================
   CALCULAR VIAJE
   ============================================================ */
void calcularViaje(void) {
    Vehiculo *lista;
    int n = cargarVehiculos(&lista);
    char nombre[MAX_NOMBRE];
    int idx;
    float precioCombustible = leerPrecioCombustible();
    DesgloseCosto d;
    float kmCiudad, kmAutopista, kmTotalViaje;
    float depreciacionViaje, mantenimientoViaje, seguroViaje;
    float neumaticosViaje, imprevistosViaje, combustibleViaje;
    float costoTotalViaje, porcentajeVidaNeumatico;

    listarVehiculos(lista, n);
    if (n == 0) return;

    printf("\nNombre del vehiculo a usar: ");
    limpiarBuffer();
    fgets(nombre, MAX_NOMBRE, stdin);
    nombre[strcspn(nombre, "\n")] = '\0';

    idx = buscarVehiculo(lista, n, nombre);
    if (idx == -1) {
        printf("No se encontro un vehiculo con ese nombre.\n");
        free(lista);
        return;
    }

    printf("Km del viaje en CIUDAD: ");
    scanf("%f", &kmCiudad);
    printf("Km del viaje en AUTOPISTA: ");
    scanf("%f", &kmAutopista);
    limpiarBuffer();

    calcularDesglose(lista[idx], precioCombustible, &d);

    kmTotalViaje = kmCiudad + kmAutopista;

    /* Los costos fijos (depreciacion, mantenimiento, seguro, neumaticos,
       imprevistos) valen lo mismo por km sin importar si es ciudad o
       autopista -- la unica diferencia entre vias es el combustible.
       Por eso se multiplican por el km TOTAL del viaje. */
    depreciacionViaje  = d.depreciacionPorKm  * kmTotalViaje;
    mantenimientoViaje = d.mantenimientoPorKm * kmTotalViaje;
    seguroViaje         = d.seguroPorKm        * kmTotalViaje;
    neumaticosViaje      = d.neumaticosPorKm    * kmTotalViaje;
    imprevistosViaje     = d.imprevistosPorKm   * kmTotalViaje;

    /* --- 8. Costo en combustible del viaje (si depende de la via) --- */
    combustibleViaje = (kmCiudad * d.combustibleCiudadPorKm) +
                        (kmAutopista * d.combustibleAutopistaPorKm);

    /* --- 4/5/6. Costo real total del viaje --- */
    costoTotalViaje = depreciacionViaje + mantenimientoViaje + seguroViaje +
                       neumaticosViaje + imprevistosViaje + combustibleViaje;

    /* Cuanto de la vida util de las gomas se "gasto" en este viaje */
    porcentajeVidaNeumatico = (kmTotalViaje / lista[idx].vidaUtilNeumaticosKm) * 100.0f;

    printf("\n================================================\n");
    printf(" VIAJE CON '%s' -- %.1f km (%.1f ciudad + %.1f autopista)\n",
           lista[idx].nombre, kmTotalViaje, kmCiudad, kmAutopista);
    printf("================================================\n");
    printf("Precio del combustible usado: RD$ %.2f / galon\n\n", precioCombustible);

    printf("De donde sale el costo del viaje:\n");
    printf("------------------------------------------------\n");
    printf("  %-30s RD$ %10.2f\n", "Depreciacion (amortizacion)", depreciacionViaje);
    printf("  %-30s RD$ %10.2f\n", "Mantenimiento", mantenimientoViaje);
    printf("  %-30s RD$ %10.2f\n", "Seguro", seguroViaje);
    printf("  %-30s RD$ %10.2f\n", "Desgaste de neumaticos", neumaticosViaje);
    printf("  %-30s RD$ %10.2f\n", "Imprevistos", imprevistosViaje);
    printf("  %-30s RD$ %10.2f\n", "Combustible", combustibleViaje);
    printf("------------------------------------------------\n");
    printf("  %-30s RD$ %10.2f\n", "COSTO REAL TOTAL DEL VIAJE", costoTotalViaje);
    printf("------------------------------------------------\n\n");

    printf("Detalle de neumaticos:\n");
    printf("  Este viaje consume %.1f km de los %d km de vida\n",
           kmTotalViaje, lista[idx].vidaUtilNeumaticosKm);
    printf("  util del juego de gomas actual (%.2f%% de su vida util).\n",
           porcentajeVidaNeumatico);

    printf("\nCosto real por km:\n");
    printf("  Ciudad:    RD$ %.2f/km\n", d.realCiudadPorKm);
    printf("  Autopista: RD$ %.2f/km\n", d.realAutopistaPorKm);

    free(lista);
}

/* ============================================================
   MENU PRINCIPAL
   ============================================================ */
void menuPrincipal(void) {
    int opcion;

    do {
        printf("\n=========================================\n");
        printf("   COSTO REAL DE VEHICULOS POR KM\n");
        printf("=========================================\n");
        printf("1. Crear vehiculo\n");
        printf("2. Borrar vehiculo\n");
        printf("3. Modificar vehiculo\n");
        printf("4. Calcular viaje\n");
        printf("5. Modificar precio del combustible\n");
        printf("6. Listar vehiculos\n");
        printf("0. Salir\n");
        printf("Seleccione una opcion: ");

        if (scanf("%d", &opcion) != 1) {
            limpiarBuffer();
            opcion = -1;
        }

        switch (opcion) {
            case 1: crearVehiculo(); pausar(); break;
            case 2: borrarVehiculo(); pausar(); break;
            case 3: modificarVehiculo(); pausar(); break;
            case 4: calcularViaje(); pausar(); break;
            case 5: modificarPrecioCombustible(); pausar(); break;
            case 6: {
                Vehiculo *lista;
                int n = cargarVehiculos(&lista);
                listarVehiculos(lista, n);
                free(lista);
                pausar();
                break;
            }
            case 0: printf("Hasta luego.\n"); break;
            default: printf("Opcion invalida.\n"); pausar();
        }
    } while (opcion != 0);
}
