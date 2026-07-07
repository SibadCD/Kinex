/* ==========================================================================
 * Archivo      : cmdb.c
 * Descripcion  : Implementacion del nucleo del modulo CMDB: creacion y
 *                liberacion de la lista, insercion ordenada, busqueda,
 *                modificacion, eliminacion y generacion de reportes sobre
 *                una lista doblemente enlazada de Elementos de Configuracion.
 *
 * Decision de diseno: la lista se mantiene siempre ORDENADA por "codigo"
 * (orden alfanumerico ascendente). Insertar cuesta O(n) en el peor caso,
 * pero a cambio la busqueda puede cortar en cuanto el codigo recorrido es
 * mayor al buscado (no hace falta recorrer toda la lista) y "mostrarTodos"
 * y "generarReportePorTipo" entregan los datos ya ordenados sin necesidad
 * de un paso adicional de ordenamiento.
 * ==========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "cmdb.h"
#include "interfaz.h"

/* Pone la lista en un estado valido y vacio. Debe llamarse antes de usar
   cualquier otra funcion del modulo. */
void inicializarLista(ListaCMDB *lista) {
    lista->cabeza = NULL;
    lista->cola = NULL;
    lista->totalElementos = 0;
}

/* --------------------------------------------------------------------------
 * crearNodo
 * Reserva memoria dinamica (malloc) para un nuevo nodo ElementoCI y copia
 * en el los datos recibidos por valor. Los punteros anterior/siguiente se
 * inicializan en NULL porque el nodo aun no pertenece a ninguna lista.
 * Retorna NULL si malloc falla (memoria agotada), lo cual es verificado
 * por quien llama a esta funcion.
 * ------------------------------------------------------------------------ */
ElementoCI *crearNodo(ElementoCI datos) {
    ElementoCI *nodo = (ElementoCI *) malloc(sizeof(ElementoCI));
    if (nodo == NULL) return NULL;     // sin memoria disponible

    *nodo = datos;          // copia todos los campos de la estructura
    nodo->anterior = NULL;
    nodo->siguiente = NULL;
    return nodo;
}

/* Recorre la lista comparando codigos; retorna 1 si ya existe, 0 si no. */
int existeCodigo(const ListaCMDB *lista, const char *codigo) {
    ElementoCI *actual = lista->cabeza;
    while (actual != NULL) {
        if (strcmp(actual->codigo, codigo) == 0) return 1;
        actual = actual->siguiente;
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * insertarOrdenado
 * Inserta "nodoNuevo" en la posicion que conserva el orden ascendente por
 * codigo, reescribiendo los cuatro punteros involucrados (anterior y
 * siguiente del nuevo nodo, y los punteros de sus vecinos). Contempla los
 * tres casos posibles: lista vacia, insercion al final y insercion en medio
 * (que incluye el caso de convertirse en la nueva cabeza).
 * ------------------------------------------------------------------------ */
void insertarOrdenado(ListaCMDB *lista, ElementoCI *nodoNuevo) {
    // Caso 1: lista vacia -> el nuevo nodo es cabeza y cola a la vez.
    if (lista->cabeza == NULL) {
        lista->cabeza = nodoNuevo;
        lista->cola = nodoNuevo;
        lista->totalElementos++;
        return;
    }

    ElementoCI *actual = lista->cabeza;

    // Avanza mientras el codigo actual sea menor al del nuevo elemento.
    while (actual != NULL && strcmp(actual->codigo, nodoNuevo->codigo) < 0) {
        actual = actual->siguiente;
    }

    if (actual == NULL) {
        /* Caso 2: se llego al final -> el nuevo nodo se agrega despues
           de la cola actual. */
        nodoNuevo->anterior = lista->cola;
        lista->cola->siguiente = nodoNuevo;
        lista->cola = nodoNuevo;
    } else if (actual == lista->cabeza) {
        // Caso 3a: el nuevo nodo pasa a ser la nueva cabeza de la lista.
        nodoNuevo->siguiente = actual;
        actual->anterior = nodoNuevo;
        lista->cabeza = nodoNuevo;
    } else {
        /* Caso 3b: insercion intermedia. Se enlaza el nuevo nodo entre
           "actual->anterior" y "actual". */
        ElementoCI *previo = actual->anterior;
        previo->siguiente = nodoNuevo;
        nodoNuevo->anterior = previo;
        nodoNuevo->siguiente = actual;
        actual->anterior = nodoNuevo;
    }

    lista->totalElementos++;
}

/* Valida duplicados, crea el nodo y lo inserta ordenado.
   Retorna: 1 = registrado, 0 = codigo duplicado, -1 = error de memoria. */
int registrarElemento(ListaCMDB *lista, ElementoCI datos) {
    if (existeCodigo(lista, datos.codigo)) return 0;

    ElementoCI *nodo = crearNodo(datos);
    if (nodo == NULL) return -1;

    insertarOrdenado(lista, nodo);
    return 1;
}

/* Busqueda con corte anticipado: como la lista esta ordenada, si el codigo
   recorrido ya es mayor (alfanumericamente) al buscado, no existe. */
ElementoCI *buscarPorCodigo(const ListaCMDB *lista, const char *codigo) {
    ElementoCI *actual = lista->cabeza;
    while (actual != NULL) {
        int comparacion = strcmp(actual->codigo, codigo);
        if (comparacion == 0) return actual;   // encontrado
        if (comparacion > 0) return NULL;      // ya lo hubiera pasado
        actual = actual->siguiente;
    }
    return NULL;
}

/* --------------------------------------------------------------------------
 * modificarElemento
 * Actualiza nombre, tipo, version, ubicacion y estado del elemento cuyo
 * codigo coincide. El codigo NO se modifica aqui (cambiarlo requeriria
 * reordenar la lista); si el usuario necesita cambiar el codigo se le
 * indica desde el menu que elimine y vuelva a registrar el elemento.
 * Retorna 1 si lo encontro y actualizo, 0 si el codigo no existe.
 * ------------------------------------------------------------------------ */
int modificarElemento(ListaCMDB *lista, const char *codigo, ElementoCI nuevosDatos) {
    ElementoCI *nodo = buscarPorCodigo(lista, codigo);
    if (nodo == NULL) return 0;

    strcpy(nodo->nombre, nuevosDatos.nombre);
    strcpy(nodo->tipo, nuevosDatos.tipo);
    strcpy(nodo->version, nuevosDatos.version);
    strcpy(nodo->ubicacion, nuevosDatos.ubicacion);
    strcpy(nodo->estado, nuevosDatos.estado);
    return 1;
}

/* --------------------------------------------------------------------------
 * eliminarElemento
 * Localiza el nodo por codigo y lo desenlaza reconectando sus vecinos
 * (anterior <-> siguiente), contemplando los casos borde: nodo cabeza,
 * nodo cola y nodo intermedio. Libera la memoria del nodo eliminado.
 * ------------------------------------------------------------------------ */
int eliminarElemento(ListaCMDB *lista, const char *codigo) {
    ElementoCI *nodo = buscarPorCodigo(lista, codigo);
    if (nodo == NULL) return 0;

    if (nodo->anterior != NULL) {
        nodo->anterior->siguiente = nodo->siguiente;   // no es la cabeza
    } else {
        lista->cabeza = nodo->siguiente;               // era la cabeza
    }

    if (nodo->siguiente != NULL) {
        nodo->siguiente->anterior = nodo->anterior;    // no es la cola
    } else {
        lista->cola = nodo->anterior;                  // era la cola
    }

    free(nodo);
    lista->totalElementos--;
    return 1;
}

/* Imprime, en formato tabular, todos los elementos registrados en orden
   ascendente de codigo (orden natural de la lista). */
void mostrarTodos(const ListaCMDB *lista) {
    if (lista->cabeza == NULL) {
        imprimirInfo("La CMDB no tiene elementos registrados.");
        return;
    }

    printf("\n" COLOR_NEGRITA COLOR_CIAN "%-10s %-22s %-15s %-8s %-20s %-14s" COLOR_RESET "\n",
           "CODIGO", "NOMBRE", "TIPO", "VERSION", "UBICACION", "ESTADO");
    printf(COLOR_CIAN "--------------------------------------------------------------------------------------------" COLOR_RESET "\n");

    ElementoCI *actual = lista->cabeza;
    while (actual != NULL) {
        printf("%-10s %-22s %-15s %-8s %-20s %-14s\n",
               actual->codigo, actual->nombre, actual->tipo,
               actual->version, actual->ubicacion, actual->estado);
        actual = actual->siguiente;
    }
    printf("\nTotal de elementos: %d\n", lista->totalElementos);
}

/* Filtra y muestra unicamente los elementos cuyo tipo coincide (sin
   distinguir mayusculas/minusculas). Retorna la cantidad de coincidencias. */
int generarReportePorTipo(const ListaCMDB *lista, const char *tipo) {
    int coincidencias = 0;
    ElementoCI *actual = lista->cabeza;

    printf("\n" COLOR_NEGRITA "=== Reporte de elementos de tipo: %s ===" COLOR_RESET "\n", tipo);
    printf(COLOR_CIAN "%-10s %-22s %-8s %-20s %-14s" COLOR_RESET "\n",
           "CODIGO", "NOMBRE", "VERSION", "UBICACION", "ESTADO");
    printf(COLOR_CIAN "------------------------------------------------------------------------" COLOR_RESET "\n");

    while (actual != NULL) {
        /* Comparacion insensible a mayusculas/minusculas caracter a
           caracter, evitando depender de funciones no estandar como
           strcasecmp en compiladores estrictamente C99/C11. */
        int coincide = (strlen(actual->tipo) == strlen(tipo));
        if (coincide) {
            for (size_t i = 0; i < strlen(tipo); i++) {
                if (tolower((unsigned char)actual->tipo[i]) != tolower((unsigned char)tipo[i])) {
                    coincide = 0;
                    break;
                }
            }
        }

        if (coincide) {
            printf("%-10s %-22s %-8s %-20s %-14s\n",
                   actual->codigo, actual->nombre, actual->version,
                   actual->ubicacion, actual->estado);
            coincidencias++;
        }
        actual = actual->siguiente;
    }

    if (coincidencias == 0) {
        printf("(Sin coincidencias para el tipo indicado)\n");
    } else {
        printf("\nCoincidencias encontradas: %d\n", coincidencias);
    }
    return coincidencias;
}

/* Recorre la lista liberando cada nodo con free(). Debe invocarse antes de
   finalizar el programa para evitar fugas de memoria (memory leaks). */
void liberarLista(ListaCMDB *lista) {
    ElementoCI *actual = lista->cabeza;
    while (actual != NULL) {
        ElementoCI *siguienteNodo = actual->siguiente;  // se guarda antes de liberar
        free(actual);
        actual = siguienteNodo;
    }
    lista->cabeza = NULL;
    lista->cola = NULL;
    lista->totalElementos = 0;
}
