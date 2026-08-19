#ifndef ARCHIVO_H
#define ARCHIVO_H

#include "riesgo.h"

#define MAX_RUTA 260

typedef enum {
    FORMATO_JSON,
    FORMATO_XML,
    FORMATO_CSV
} FormatoArchivo;

/* Detecta el formato a partir de la extension del nombre de archivo.
   Devuelve 1 si pudo detectarlo (y lo guarda en *formato), 0 si no. */
int formato_desde_extension(const char *ruta, FormatoArchivo *formato);

/* Crea un archivo nuevo (vacio) en la ruta indicada. Devuelve 1 si tuvo exito. */
int archivo_crear(const char *ruta);

/* Verifica si el archivo existe y se puede abrir para lectura. */
int archivo_existe(const char *ruta);

/* Carga los riesgos desde disco hacia la lista (usa memoria dinamica).
   Devuelve la cantidad de riesgos cargados, o -1 si hubo error. */
int archivo_cargar(const char *ruta, ListaRiesgos *lista);

/* Guarda los riesgos en disco, un riesgo por linea, en el formato indicado.
   Devuelve 1 si tuvo exito. */
int archivo_guardar(const char *ruta, FormatoArchivo formato, ListaRiesgos *lista);

#endif
