/* ==========================================================================
 * Archivo      : validaciones.c
 * Descripcion  : Implementa la lectura segura de cadenas desde teclado
 *                (evitando desbordamientos de buffer) y las validaciones
 *                minimas exigidas por el enunciado: "validar los datos
 *                ingresados por el usuario".
 * ==========================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "validaciones.h"

/* Descarta cualquier caracter que haya quedado pendiente en el buffer de
   entrada estandar (tipicamente el '\n' dejado por un scanf/fgets previo). */
void limpiarBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
}

/* Lee una linea completa con fgets (mas segura que scanf("%s", ...) porque
   respeta el tamano del buffer) y elimina el salto de linea final. */
void leerCadena(char *destino, int tamano) {
    if (fgets(destino, tamano, stdin) != NULL) {
        destino[strcspn(destino, "\n")] = '\0';
    } else {
        destino[0] = '\0';
    }
}

// Retorna 1 si la cadena tiene al menos un caracter distinto de espacio.
int cadenaNoVacia(const char *cadena) {
    for (size_t i = 0; i < strlen(cadena); i++) {
        if (cadena[i] != ' ') return 1;
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * estadoValido
 * La CMDB solo acepta cuatro estados definidos por el equipo (alineados a
 * ITIL: un CI esta operativo, fuera de servicio, en mantenimiento o dado
 * de baja). Se compara sin distinguir mayusculas/minusculas para no
 * penalizar al usuario por el uso de teclas Shift/Bloq Mayus.
 * ------------------------------------------------------------------------ */
int estadoValido(const char *estado) {
    const char *estadosPermitidos[] = {"Activo", "Inactivo", "Mantenimiento", "Retirado"};
    const int totalEstados = 4;

    for (int i = 0; i < totalEstados; i++) {
        // comparacion manual insensible a mayusculas (evita depender de
        // strcasecmp, que no forma parte del estandar ANSI C / ISO C).
        if (strlen(estado) != strlen(estadosPermitidos[i])) continue;

        int iguales = 1;
        for (size_t j = 0; j < strlen(estado); j++) {
            if (tolower((unsigned char)estado[j]) != tolower((unsigned char)estadosPermitidos[i][j])) {
                iguales = 0;
                break;
            }
        }
        if (iguales) return 1;
    }
    return 0;
}

/* --------------------------------------------------------------------------
 * cadenaSegura
 * El archivo de persistencia usa ';' como separador entre campos
 * (vease persistencia.c). Si el usuario ingresara ese caracter dentro de,
 * por ejemplo, el nombre de un elemento, la linea guardada tendria un
 * campo de mas y cargarDesdeArchivo() desalinearia todos los datos de ese
 * elemento en la siguiente ejecucion. Esta funcion evita ese problema
 * desde el origen, rechazando el caracter antes de aceptarlo.
 * ------------------------------------------------------------------------ */
int cadenaSegura(const char *cadena) {
    for (size_t i = 0; i < strlen(cadena); i++) {
        if (cadena[i] == ';') return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * codigoValido
 * Endurece la validacion del codigo del elemento: ademas de no estar vacio
 * y no contener ';', tampoco debe contener espacios internos (para que no
 * se desalinee al imprimirse en columnas de ancho fijo en mostrarTodos())
 * ni comenzar con '-' (para evitar codigos con apariencia de valor
 * negativo, que podrian confundirse si en el futuro se ordenan o
 * comparan de forma numerica).
 * ------------------------------------------------------------------------ */
int codigoValido(const char *codigo) {
    if (!cadenaNoVacia(codigo)) return 0;
    if (!cadenaSegura(codigo)) return 0;
    if (codigo[0] == '-') return 0;

    for (size_t i = 0; i < strlen(codigo); i++) {
        if (isspace((unsigned char)codigo[i])) return 0;
    }
    return 1;
}

/* --------------------------------------------------------------------------
 * numeroValido
 * Se usa para el campo Version. No obliga a que sea un numero puro (se
 * aceptan formatos como "2.4" o "12.0b"), pero rechaza cadenas vacias,
 * cadenas con ';' y cualquier valor que comience con '-', es decir, que
 * "parezca" un numero negativo.
 * ------------------------------------------------------------------------ */
int numeroValido(const char *cadena) {
    if (!cadenaNoVacia(cadena)) return 0;
    if (!cadenaSegura(cadena)) return 0;
    if (cadena[0] == '-') return 0;
    return 1;
}
