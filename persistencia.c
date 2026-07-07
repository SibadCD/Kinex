/* ==========================================================================
 * Archivo      : persistencia.c
 * Descripcion  : Serializa la lista doblemente enlazada hacia un archivo de
 *                texto plano (formato delimitado por ';') y la reconstruye
 *                a partir de dicho archivo al iniciar el programa.
 *
 * Formato de linea persistida:
 *   CODIGO;NOMBRE;TIPO;VERSION;UBICACION;ESTADO
 * ==========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdb.h"
#include "persistencia.h"
#include "interfaz.h"

/* Escribe cada nodo de la lista, en orden, como una linea delimitada por
   ';' dentro de "nombreArchivo". Retorna 1 si todo salio bien, 0 si el
   archivo no pudo abrirse para escritura. */
int guardarEnArchivo(const ListaCMDB *lista, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "w");
    if (archivo == NULL) {
        imprimirError("No se pudo abrir '%s' para escritura.", nombreArchivo);
        return 0;
    }

    ElementoCI *actual = lista->cabeza;
    while (actual != NULL) {
        fprintf(archivo, "%s;%s;%s;%s;%s;%s\n",
                actual->codigo, actual->nombre, actual->tipo,
                actual->version, actual->ubicacion, actual->estado);
        actual = actual->siguiente;
    }

    fclose(archivo);
    return 1;
}

/* --------------------------------------------------------------------------
 * cargarDesdeArchivo
 * Lee linea por linea el archivo indicado, separa los seis campos con
 * strtok() usando ';' como delimitador y reconstruye cada Elemento de
 * Configuracion, insertandolo en la lista mediante insertarOrdenado().
 * Si el archivo no existe todavia (primera ejecucion del programa), la
 * funcion simplemente retorna 0 sin producir un error fatal: la CMDB
 * arranca vacia.
 * ------------------------------------------------------------------------ */
int cargarDesdeArchivo(ListaCMDB *lista, const char *nombreArchivo) {
    FILE *archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        return 0;   // no existe aun: no es un error, la CMDB inicia vacia
    }

    char linea[TAM_LINEA];
    int registrosLeidos = 0;

    while (fgets(linea, TAM_LINEA, archivo) != NULL) {
        linea[strcspn(linea, "\n")] = '\0';   // elimina el salto de linea final
        if (strlen(linea) == 0) continue;      // ignora lineas vacias

        ElementoCI datos;
        memset(&datos, 0, sizeof(ElementoCI));

        char *token = strtok(linea, ";");
        if (token != NULL) strncpy(datos.codigo, token, TAM_CODIGO - 1);

        token = strtok(NULL, ";");
        if (token != NULL) strncpy(datos.nombre, token, TAM_NOMBRE - 1);

        token = strtok(NULL, ";");
        if (token != NULL) strncpy(datos.tipo, token, TAM_TIPO - 1);

        token = strtok(NULL, ";");
        if (token != NULL) strncpy(datos.version, token, TAM_VERSION - 1);

        token = strtok(NULL, ";");
        if (token != NULL) strncpy(datos.ubicacion, token, TAM_UBICACION - 1);

        token = strtok(NULL, ";");
        if (token != NULL) strncpy(datos.estado, token, TAM_ESTADO - 1);

        ElementoCI *nodo = crearNodo(datos);
        if (nodo != NULL) {
            insertarOrdenado(lista, nodo);
            registrosLeidos++;
        }
    }

    fclose(archivo);
    return registrosLeidos;
}
