#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "riesgo.h"

#define CAPACIDAD_INICIAL 4

void lista_iniciar(ListaRiesgos *lista) {
    lista->items = NULL;
    lista->cantidad = 0;
    lista->capacidad = 0;
}

void lista_liberar(ListaRiesgos *lista) {
    free(lista->items);          /* libera la memoria dinamica */
    lista->items = NULL;
    lista->cantidad = 0;
    lista->capacidad = 0;
}

/* Duplica la capacidad del arreglo dinamico cuando se llena (realloc) */
static int lista_asegurar_capacidad(ListaRiesgos *lista) {
    if (lista->cantidad < lista->capacidad) return 1;

    int nueva_capacidad = (lista->capacidad == 0) ? CAPACIDAD_INICIAL : lista->capacidad * 2;
    Riesgo *nuevo = (Riesgo *) realloc(lista->items, (size_t) nueva_capacidad * sizeof(Riesgo));
    if (nuevo == NULL) {
        fprintf(stderr, "Error: no hay memoria suficiente (realloc)\n");
        return 0;
    }
    lista->items = nuevo;
    lista->capacidad = nueva_capacidad;
    return 1;
}

int siguiente_id(ListaRiesgos *lista) {
    int max_id = 0;
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->items[i].id > max_id) max_id = lista->items[i].id;
    }
    return max_id + 1;
}

Riesgo *lista_agregar(ListaRiesgos *lista) {
    if (!lista_asegurar_capacidad(lista)) return NULL;
    Riesgo *r = &lista->items[lista->cantidad];
    memset(r, 0, sizeof(Riesgo));
    lista->cantidad++;
    return r;
}

Riesgo *lista_buscar_por_id(ListaRiesgos *lista, int id) {
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->items[i].id == id) return &lista->items[i];
    }
    return NULL;
}

int lista_borrar_por_id(ListaRiesgos *lista, int id) {
    for (int i = 0; i < lista->cantidad; i++) {
        if (lista->items[i].id == id) {
            for (int j = i; j < lista->cantidad - 1; j++) {
                lista->items[j] = lista->items[j + 1];
            }
            lista->cantidad--;
            return 1;
        }
    }
    return 0;
}

const char *riesgo_nivel_texto(int gr) {
    /* Tabla de clasificacion del metodo Mosler (rango real: 2 a 1250).
       Verifica con el material de tu profesor por si usa otros cortes o
       nombres (algunas fuentes usan "Muy Reducida...Muy Elevada" en vez
       de "Muy Bajo...Elevado", pero los cortes numericos suelen coincidir). */
    if (gr <= 250)  return "Muy Bajo";
    if (gr <= 500)  return "Pequeno";
    if (gr <= 750)  return "Normal";
    if (gr <= 1000) return "Grande";
    return "Elevado";
}

void riesgo_calcular(Riesgo *r) {
    int I = r->F * r->S;   /* Importancia del suceso */
    int D = r->P * r->E;   /* Danos ocasionados */
    r->C  = I + D;         /* Caracter */
    r->Pr = r->A * r->V;   /* Probabilidad */
    r->GR = r->C * r->Pr;  /* Riesgo Estimado (ER) */
    strncpy(r->nivel, riesgo_nivel_texto(r->GR), MAX_NIVEL - 1);
    r->nivel[MAX_NIVEL - 1] = '\0';
}

void riesgo_calcular_todos(ListaRiesgos *lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        riesgo_calcular(&lista->items[i]);
    }
}
