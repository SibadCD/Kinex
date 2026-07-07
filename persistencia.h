/* ==========================================================================
 * Archivo      : persistencia.h
 * Descripcion  : Prototipos para guardar y cargar la lista de Elementos de
 *                Configuracion desde/hacia un archivo de texto plano, lo
 *                que garantiza que la informacion sobreviva entre distintas
 *                ejecuciones del programa (requisito de persistencia).
 * ==========================================================================
 */

#ifndef PERSISTENCIA_H
#define PERSISTENCIA_H

#include "cmdb.h"

int guardarEnArchivo(const ListaCMDB *lista, const char *nombreArchivo);
int cargarDesdeArchivo(ListaCMDB *lista, const char *nombreArchivo);

#endif /* PERSISTENCIA_H */
