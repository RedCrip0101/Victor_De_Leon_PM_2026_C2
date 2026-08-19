#ifndef RIESGO_H
#define RIESGO_H

#define MAX_NOMBRE 100
#define MAX_DESC   250
#define MAX_NIVEL  20

/*
 * Metodo Mosler:
 *   F = Funcion        (importancia de la funcion afectada)      1-5
 *   S = Sustitucion     (facilidad para sustituir lo afectado)    1-5
 *   P = Profundidad     (perturbacion / gravedad del dano)        1-5
 *   E = Extension       (alcance del dano)                        1-5
 *   A = Agresion        (probabilidad de que ocurra)               1-5
 *   V = Vulnerabilidad  (probabilidad de que el dano se produzca)  1-5
 *
 *   I  (Importancia del suceso) = F * S
 *   D  (Danos ocasionados)      = P * E
 *   C  (Caracter)                = I + D          -> rango 2-50
 *   Pr (Probabilidad)            = A * V            -> rango 1-25
 *   GR (Riesgo Estimado, ER)     = C * Pr            -> rango 2-1250
 */
typedef struct {
    int id;
    char nombre[MAX_NOMBRE];
    char descripcion[MAX_DESC];
    int F, S, P, E, A, V;
    int C;
    int Pr;
    int GR;
    char nivel[MAX_NIVEL];
} Riesgo;

typedef struct {
    Riesgo *items;
    int cantidad;
    int capacidad;
} ListaRiesgos;

void      lista_iniciar(ListaRiesgos *lista);
void      lista_liberar(ListaRiesgos *lista);
Riesgo   *lista_agregar(ListaRiesgos *lista);
int       lista_borrar_por_id(ListaRiesgos *lista, int id);
Riesgo   *lista_buscar_por_id(ListaRiesgos *lista, int id);
int       siguiente_id(ListaRiesgos *lista);

void        riesgo_calcular(Riesgo *r);
void        riesgo_calcular_todos(ListaRiesgos *lista);
const char *riesgo_nivel_texto(int gr);

#endif
