#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riesgo.h"
#include "archivo.h"

/* ---------- entrada de datos segura ---------- */

static void limpiar_buffer_entrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static void leer_linea(const char *prompt, char *destino, int max_len) {
    printf("%s", prompt);
    if (fgets(destino, max_len, stdin) == NULL) { destino[0] = '\0'; return; }
    size_t len = strlen(destino);
    if (len > 0 && destino[len - 1] == '\n') destino[len - 1] = '\0';
    else limpiar_buffer_entrada(); /* la linea era mas larga que el buffer */
}

static int leer_entero_rango(const char *prompt, int minimo, int maximo) {
    char linea[64];
    int valor;
    while (1) {
        leer_linea(prompt, linea, sizeof(linea));
        if (sscanf(linea, "%d", &valor) == 1 && valor >= minimo && valor <= maximo) {
            return valor;
        }
        printf("  -> Valor invalido. Debe estar entre %d y %d.\n", minimo, maximo);
    }
}

static int leer_entero(const char *prompt) {
    char linea[64];
    int valor;
    while (1) {
        leer_linea(prompt, linea, sizeof(linea));
        if (sscanf(linea, "%d", &valor) == 1) return valor;
        printf("  -> Valor invalido, escribe un numero.\n");
    }
}

/* ---------- estado del programa ---------- */

typedef struct {
    ListaRiesgos lista;
    char archivo_actual[MAX_RUTA];
    int  tiene_archivo;
} Estado;

/* ---------- operaciones del menu ---------- */

static void accion_crear_riesgo(Estado *e) {
    Riesgo *r = lista_agregar(&e->lista);
    if (!r) { printf("No se pudo crear el riesgo (memoria).\n"); return; }

    r->id = siguiente_id(&e->lista);
    printf("\n== Nuevo riesgo (id %d) ==\n", r->id);
    leer_linea("Nombre: ", r->nombre, MAX_NOMBRE);
    leer_linea("Descripcion: ", r->descripcion, MAX_DESC);

    printf("Factores del metodo Mosler (escala 1 a 5):\n");
    r->F = leer_entero_rango("  F - Funcion: ", 1, 5);
    r->S = leer_entero_rango("  S - Sustitucion: ", 1, 5);
    r->P = leer_entero_rango("  P - Profundidad: ", 1, 5);
    r->E = leer_entero_rango("  E - Extension: ", 1, 5);
    r->A = leer_entero_rango("  A - Agresion: ", 1, 5);
    r->V = leer_entero_rango("  V - Vulnerabilidad: ", 1, 5);

    riesgo_calcular(r);
    printf("Riesgo creado y calculado (GR = %d, nivel: %s).\n", r->GR, r->nivel);
}

static void accion_modificar_riesgo(Estado *e) {
    if (e->lista.cantidad == 0) { printf("No hay riesgos cargados.\n"); return; }

    int id = leer_entero("ID del riesgo a modificar: ");
    Riesgo *r = lista_buscar_por_id(&e->lista, id);
    if (!r) { printf("No existe un riesgo con ese ID.\n"); return; }

    printf("Deja el campo vacio para conservar el valor actual.\n");

    char buf[MAX_DESC];
    leer_linea("Nombre nuevo: ", buf, sizeof(buf));
    if (buf[0] != '\0') strncpy(r->nombre, buf, MAX_NOMBRE - 1);

    leer_linea("Descripcion nueva: ", buf, sizeof(buf));
    if (buf[0] != '\0') strncpy(r->descripcion, buf, MAX_DESC - 1);

    r->F = leer_entero_rango("  F - Funcion: ", 1, 5);
    r->S = leer_entero_rango("  S - Sustitucion: ", 1, 5);
    r->P = leer_entero_rango("  P - Profundidad: ", 1, 5);
    r->E = leer_entero_rango("  E - Extension: ", 1, 5);
    r->A = leer_entero_rango("  A - Agresion: ", 1, 5);
    r->V = leer_entero_rango("  V - Vulnerabilidad: ", 1, 5);

    riesgo_calcular(r);
    printf("Riesgo actualizado (GR = %d, nivel: %s).\n", r->GR, r->nivel);
}

static void accion_calcular_riesgos(Estado *e) {
    if (e->lista.cantidad == 0) { printf("No hay riesgos cargados.\n"); return; }
    riesgo_calcular_todos(&e->lista);
    printf("Se recalcularon %d riesgo(s).\n", e->lista.cantidad);
}

static void accion_imprimir_riesgos(Estado *e) {
    if (e->lista.cantidad == 0) { printf("No hay riesgos cargados.\n"); return; }

    printf("\n%-4s %-20s %-4s %-4s %-4s %-15s\n", "ID", "Nombre", "C", "Pr", "GR", "Nivel");
    printf("---------------------------------------------------------\n");
    for (int i = 0; i < e->lista.cantidad; i++) {
        Riesgo *r = &e->lista.items[i];
        printf("%-4d %-20.20s %-4d %-4d %-4d %-15s\n", r->id, r->nombre, r->C, r->Pr, r->GR, r->nivel);
    }
    printf("\n");
}

static void accion_borrar_riesgo(Estado *e) {
    if (e->lista.cantidad == 0) { printf("No hay riesgos cargados.\n"); return; }

    int id = leer_entero("ID del riesgo a borrar: ");
    if (lista_borrar_por_id(&e->lista, id)) {
        printf("Riesgo %d eliminado.\n", id);
    } else {
        printf("No existe un riesgo con ese ID.\n");
    }
}

static void accion_seleccionar_archivo(Estado *e) {
    char ruta[MAX_RUTA];
    leer_linea("Ruta del archivo a seleccionar: ", ruta, sizeof(ruta));

    FormatoArchivo f;
    if (!formato_desde_extension(ruta, &f)) {
        printf("Extension no reconocida. Usa .json, .xml o .csv\n");
        return;
    }
    if (!archivo_existe(ruta)) {
        printf("Ese archivo no existe. Puedes crearlo con la opcion 7.\n");
        return;
    }

    strncpy(e->archivo_actual, ruta, MAX_RUTA - 1);
    e->tiene_archivo = 1;
    printf("Archivo activo: %s\n", e->archivo_actual);
}

static void accion_crear_archivo(Estado *e) {
    char ruta[MAX_RUTA];
    leer_linea("Nombre del archivo a crear (ej: riesgos.json): ", ruta, sizeof(ruta));

    FormatoArchivo f;
    if (!formato_desde_extension(ruta, &f)) {
        printf("Extension no reconocida. Usa .json, .xml o .csv\n");
        return;
    }
    if (!archivo_crear(ruta)) {
        printf("No se pudo crear el archivo.\n");
        return;
    }

    strncpy(e->archivo_actual, ruta, MAX_RUTA - 1);
    e->tiene_archivo = 1;
    printf("Archivo '%s' creado y seleccionado como activo.\n", ruta);
}

static void accion_cargar_desde_disco(Estado *e) {
    if (!e->tiene_archivo) { printf("Primero selecciona o crea un archivo (opciones 6 o 7).\n"); return; }

    lista_liberar(&e->lista);   /* libera lo que hubiera en memoria */
    lista_iniciar(&e->lista);

    int n = archivo_cargar(e->archivo_actual, &e->lista);
    if (n < 0) {
        printf("No se pudo leer '%s' (puede estar vacio o no existir aun).\n", e->archivo_actual);
        return;
    }
    printf("Se cargaron %d riesgo(s) desde '%s' usando memoria dinamica.\n", n, e->archivo_actual);
}

static void accion_guardar_en_disco(Estado *e) {
    if (!e->tiene_archivo) { printf("Primero selecciona o crea un archivo (opciones 6 o 7).\n"); return; }
    if (e->lista.cantidad == 0) { printf("No hay riesgos para guardar.\n"); return; }

    FormatoArchivo f;
    formato_desde_extension(e->archivo_actual, &f); /* ya validado al seleccionar/crear */

    riesgo_calcular_todos(&e->lista);
    if (archivo_guardar(e->archivo_actual, f, &e->lista)) {
        printf("Se guardaron %d riesgo(s) en '%s' (una linea por riesgo).\n", e->lista.cantidad, e->archivo_actual);
    } else {
        printf("No se pudo guardar el archivo.\n");
    }
}

/* ---------- menu principal ---------- */

static void mostrar_menu(const Estado *e) {
    printf("\n============================================\n");
    printf(" GESTION DE RIESGOS - METODO MOSLER\n");
    printf(" Archivo activo: %s\n", e->tiene_archivo ? e->archivo_actual : "(ninguno)");
    printf("============================================\n");
    printf(" 1. Crear riesgo\n");
    printf(" 2. Modificar parametros de un riesgo\n");
    printf(" 3. Calcular riesgos (metodo Mosler)\n");
    printf(" 4. Imprimir riesgos\n");
    printf(" 5. Borrar riesgo\n");
    printf(" 6. Seleccionar un archivo especifico\n");
    printf(" 7. Crear un archivo especifico\n");
    printf(" 8. Cargar riesgos desde disco (memoria dinamica)\n");
    printf(" 9. Guardar riesgos en disco (json/xml/csv)\n");
    printf(" 0. Salir\n");
}

int main(void) {
    Estado e;
    lista_iniciar(&e.lista);
    e.tiene_archivo = 0;
    e.archivo_actual[0] = '\0';

    int opcion;
    do {
        mostrar_menu(&e);
        opcion = leer_entero_rango("Selecciona una opcion: ", 0, 9);

        switch (opcion) {
            case 1: accion_crear_riesgo(&e); break;
            case 2: accion_modificar_riesgo(&e); break;
            case 3: accion_calcular_riesgos(&e); break;
            case 4: accion_imprimir_riesgos(&e); break;
            case 5: accion_borrar_riesgo(&e); break;
            case 6: accion_seleccionar_archivo(&e); break;
            case 7: accion_crear_archivo(&e); break;
            case 8: accion_cargar_desde_disco(&e); break;
            case 9: accion_guardar_en_disco(&e); break;
            case 0: printf("Hasta luego.\n"); break;
        }
    } while (opcion != 0);

    lista_liberar(&e.lista);
    return 0;
}
