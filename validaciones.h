/* ==========================================================================
 * Archivo      : validaciones.h
 * Descripcion  : Funciones de apoyo para leer y validar los datos que el
 *                usuario ingresa por teclado, evitando que la CMDB quede
 *                con informacion vacia, incompleta o en un estado invalido.
 * ==========================================================================
 */

#ifndef VALIDACIONES_H
#define VALIDACIONES_H

void limpiarBufferEntrada(void);
void leerCadena(char *destino, int tamano);
int cadenaNoVacia(const char *cadena);
int estadoValido(const char *estado);

/* Nuevas validaciones (endurecimiento contra datos que podrian romper
   el programa mas adelante): */
int cadenaSegura(const char *cadena);      // rechaza ';' (delimitador del archivo)
int codigoValido(const char *codigo);      // no vacio, seguro, sin espacios ni signo '-'
int numeroValido(const char *cadena);      // no vacio, seguro, y si "parece" numerico no es negativo

#endif /* VALIDACIONES_H */
