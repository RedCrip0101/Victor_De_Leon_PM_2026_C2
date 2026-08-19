#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>   /* strcasecmp */
#include <ctype.h>
#include "archivo.h"
#include "cJSON.h"

/* ---------- utilidades generales ---------- */

int formato_desde_extension(const char *ruta, FormatoArchivo *formato) {
    const char *punto = strrchr(ruta, '.');
    if (punto == NULL) return 0;

    if (strcasecmp(punto, ".json") == 0) { *formato = FORMATO_JSON; return 1; }
    if (strcasecmp(punto, ".xml")  == 0) { *formato = FORMATO_XML;  return 1; }
    if (strcasecmp(punto, ".csv")  == 0) { *formato = FORMATO_CSV;  return 1; }
    return 0;
}

int archivo_existe(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

int archivo_crear(const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) return 0;
    fclose(f);
    return 1;
}

/* Quita el salto de linea final que deja fgets */
static void quitar_salto_linea(char *linea) {
    size_t len = strlen(linea);
    while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r')) {
        linea[--len] = '\0';
    }
}

/* ---------- CSV ---------- */

/* Escribe un campo CSV, encerrandolo en comillas si contiene coma o comillas */
static void escribir_campo_csv(FILE *f, const char *valor, int es_ultimo) {
    int necesita_comillas = (strchr(valor, ',') != NULL) || (strchr(valor, '"') != NULL);
    if (necesita_comillas) {
        fputc('"', f);
        for (const char *p = valor; *p; p++) {
            if (*p == '"') fputc('"', f); /* duplicar comillas internas */
            fputc(*p, f);
        }
        fputc('"', f);
    } else {
        fputs(valor, f);
    }
    fputc(es_ultimo ? '\n' : ',', f);
}

/* Separa una linea CSV en hasta n_campos, respetando comillas. */
static int separar_linea_csv(const char *linea, char campos[][MAX_DESC], int n_campos) {
    int campo = 0, i = 0;
    const char *p = linea;

    while (*p && campo < n_campos) {
        int j = 0;
        if (*p == '"') {
            p++;
            while (*p && j < MAX_DESC - 1) {
                if (*p == '"' && *(p + 1) == '"') { campos[campo][j++] = '"'; p += 2; continue; }
                if (*p == '"') { p++; break; }
                campos[campo][j++] = *p++;
            }
        } else {
            while (*p && *p != ',' && j < MAX_DESC - 1) campos[campo][j++] = *p++;
        }
        campos[campo][j] = '\0';
        campo++;
        if (*p == ',') p++;
    }
    (void) i;
    return campo;
}

static void guardar_csv(FILE *f, ListaRiesgos *lista) {
    fprintf(f, "id,nombre,descripcion,F,S,P,E,A,V,C,Pr,GR,nivel\n");
    for (int i = 0; i < lista->cantidad; i++) {
        Riesgo *r = &lista->items[i];
        char buf[16];

        snprintf(buf, sizeof(buf), "%d", r->id);          escribir_campo_csv(f, buf, 0);
        escribir_campo_csv(f, r->nombre, 0);
        escribir_campo_csv(f, r->descripcion, 0);
        snprintf(buf, sizeof(buf), "%d", r->F);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->S);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->P);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->E);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->A);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->V);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->C);            escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->Pr);           escribir_campo_csv(f, buf, 0);
        snprintf(buf, sizeof(buf), "%d", r->GR);           escribir_campo_csv(f, buf, 0);
        escribir_campo_csv(f, r->nivel, 1);
    }
}

static int cargar_csv(FILE *f, ListaRiesgos *lista) {
    char linea[1024];
    int primera = 1;
    int cargados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        quitar_salto_linea(linea);
        if (linea[0] == '\0') continue;
        if (primera) { primera = 0; continue; } /* saltar encabezado */

        char campos[13][MAX_DESC];
        int n = separar_linea_csv(linea, campos, 13);
        if (n < 13) continue;

        Riesgo *r = lista_agregar(lista);
        if (!r) return -1;

        r->id = atoi(campos[0]);
        strncpy(r->nombre, campos[1], MAX_NOMBRE - 1);
        strncpy(r->descripcion, campos[2], MAX_DESC - 1);
        r->F = atoi(campos[3]);
        r->S = atoi(campos[4]);
        r->P = atoi(campos[5]);
        r->E = atoi(campos[6]);
        r->A = atoi(campos[7]);
        r->V = atoi(campos[8]);
        riesgo_calcular(r); /* recalcula C, Pr, GR, nivel por consistencia */
        cargados++;
    }
    return cargados;
}

/* ---------- XML ---------- */

static void escribir_texto_xml_escapado(FILE *f, const char *valor) {
    for (const char *p = valor; *p; p++) {
        switch (*p) {
            case '&':  fputs("&amp;", f);  break;
            case '<':  fputs("&lt;", f);   break;
            case '>':  fputs("&gt;", f);   break;
            case '"':  fputs("&quot;", f); break;
            default:   fputc(*p, f);
        }
    }
}

static void guardar_xml(FILE *f, ListaRiesgos *lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        Riesgo *r = &lista->items[i];
        fprintf(f, "<riesgo><id>%d</id><nombre>", r->id);
        escribir_texto_xml_escapado(f, r->nombre);
        fprintf(f, "</nombre><descripcion>");
        escribir_texto_xml_escapado(f, r->descripcion);
        fprintf(f, "</descripcion><F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>"
                   "<C>%d</C><Pr>%d</Pr><GR>%d</GR><nivel>%s</nivel></riesgo>\n",
                r->F, r->S, r->P, r->E, r->A, r->V, r->C, r->Pr, r->GR, r->nivel);
    }
}

/* Extrae el contenido entre <tag>...</tag> de una linea; desescapa entidades basicas. */
static int extraer_tag_xml(const char *linea, const char *tag, char *destino, int max_len) {
    char apertura[32], cierre[32];
    snprintf(apertura, sizeof(apertura), "<%s>", tag);
    snprintf(cierre, sizeof(cierre), "</%s>", tag);

    const char *inicio = strstr(linea, apertura);
    if (!inicio) return 0;
    inicio += strlen(apertura);
    const char *fin = strstr(inicio, cierre);
    if (!fin) return 0;

    int j = 0;
    const char *p = inicio;
    while (p < fin && j < max_len - 1) {
        if (strncmp(p, "&amp;", 5) == 0)      { destino[j++] = '&'; p += 5; }
        else if (strncmp(p, "&lt;", 4) == 0)  { destino[j++] = '<'; p += 4; }
        else if (strncmp(p, "&gt;", 4) == 0)  { destino[j++] = '>'; p += 4; }
        else if (strncmp(p, "&quot;", 6) == 0){ destino[j++] = '"'; p += 6; }
        else destino[j++] = *p++;
    }
    destino[j] = '\0';
    return 1;
}

static int cargar_xml(FILE *f, ListaRiesgos *lista) {
    char linea[2048];
    char valor[MAX_DESC];
    int cargados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        quitar_salto_linea(linea);
        if (strstr(linea, "<riesgo>") == NULL) continue;

        Riesgo *r = lista_agregar(lista);
        if (!r) return -1;

        if (extraer_tag_xml(linea, "id", valor, sizeof(valor)))          r->id = atoi(valor);
        if (extraer_tag_xml(linea, "nombre", valor, sizeof(valor)))      strncpy(r->nombre, valor, MAX_NOMBRE - 1);
        if (extraer_tag_xml(linea, "descripcion", valor, sizeof(valor))) strncpy(r->descripcion, valor, MAX_DESC - 1);
        if (extraer_tag_xml(linea, "F", valor, sizeof(valor)))           r->F = atoi(valor);
        if (extraer_tag_xml(linea, "S", valor, sizeof(valor)))           r->S = atoi(valor);
        if (extraer_tag_xml(linea, "P", valor, sizeof(valor)))           r->P = atoi(valor);
        if (extraer_tag_xml(linea, "E", valor, sizeof(valor)))           r->E = atoi(valor);
        if (extraer_tag_xml(linea, "A", valor, sizeof(valor)))           r->A = atoi(valor);
        if (extraer_tag_xml(linea, "V", valor, sizeof(valor)))           r->V = atoi(valor);

        riesgo_calcular(r);
        cargados++;
    }
    return cargados;
}

/* ---------- JSON (JSON Lines: un objeto JSON por linea) ---------- */

static void guardar_json(FILE *f, ListaRiesgos *lista) {
    for (int i = 0; i < lista->cantidad; i++) {
        Riesgo *r = &lista->items[i];
        cJSON *obj = cJSON_CreateObject();
        cJSON_AddNumberToObject(obj, "id", r->id);
        cJSON_AddStringToObject(obj, "nombre", r->nombre);
        cJSON_AddStringToObject(obj, "descripcion", r->descripcion);
        cJSON_AddNumberToObject(obj, "F", r->F);
        cJSON_AddNumberToObject(obj, "S", r->S);
        cJSON_AddNumberToObject(obj, "P", r->P);
        cJSON_AddNumberToObject(obj, "E", r->E);
        cJSON_AddNumberToObject(obj, "A", r->A);
        cJSON_AddNumberToObject(obj, "V", r->V);
        cJSON_AddNumberToObject(obj, "C", r->C);
        cJSON_AddNumberToObject(obj, "Pr", r->Pr);
        cJSON_AddNumberToObject(obj, "GR", r->GR);
        cJSON_AddStringToObject(obj, "nivel", r->nivel);

        char *texto = cJSON_PrintUnformatted(obj); /* una linea, compacto */
        fprintf(f, "%s\n", texto);

        cJSON_free(texto);
        cJSON_Delete(obj);
    }
}

static int cargar_json(FILE *f, ListaRiesgos *lista) {
    char linea[2048];
    int cargados = 0;

    while (fgets(linea, sizeof(linea), f)) {
        quitar_salto_linea(linea);
        if (linea[0] == '\0') continue;

        cJSON *obj = cJSON_Parse(linea);
        if (obj == NULL) continue; /* linea invalida: se ignora */

        Riesgo *r = lista_agregar(lista);
        if (!r) { cJSON_Delete(obj); return -1; }

        cJSON *item;
        if ((item = cJSON_GetObjectItem(obj, "id")))          r->id = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "nombre")))      strncpy(r->nombre, item->valuestring, MAX_NOMBRE - 1);
        if ((item = cJSON_GetObjectItem(obj, "descripcion"))) strncpy(r->descripcion, item->valuestring, MAX_DESC - 1);
        if ((item = cJSON_GetObjectItem(obj, "F")))           r->F = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "S")))           r->S = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "P")))           r->P = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "E")))           r->E = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "A")))           r->A = item->valueint;
        if ((item = cJSON_GetObjectItem(obj, "V")))           r->V = item->valueint;

        riesgo_calcular(r);
        cJSON_Delete(obj);
        cargados++;
    }
    return cargados;
}

/* ---------- API publica ---------- */

int archivo_guardar(const char *ruta, FormatoArchivo formato, ListaRiesgos *lista) {
    FILE *f = fopen(ruta, "w");
    if (f == NULL) return 0;

    switch (formato) {
        case FORMATO_JSON: guardar_json(f, lista); break;
        case FORMATO_XML:  guardar_xml(f, lista);  break;
        case FORMATO_CSV:  guardar_csv(f, lista);  break;
    }

    fclose(f);
    return 1;
}

int archivo_cargar(const char *ruta, ListaRiesgos *lista) {
    FormatoArchivo formato;
    if (!formato_desde_extension(ruta, &formato)) return -1;

    FILE *f = fopen(ruta, "r");
    if (f == NULL) return -1;

    int cargados;
    switch (formato) {
        case FORMATO_JSON: cargados = cargar_json(f, lista); break;
        case FORMATO_XML:  cargados = cargar_xml(f, lista);  break;
        case FORMATO_CSV:  cargados = cargar_csv(f, lista);  break;
        default:           cargados = -1;
    }

    fclose(f);
    return cargados;
}
