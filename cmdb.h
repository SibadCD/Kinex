/* ==========================================================================
 * Archivo      : cmdb.h
 * Proyecto     : Sistema Integral de Gestion de Servicios TI - TechSolutions S.A.
 * Modulo       : Grupo 6 - Gestion de Configuracion (CMDB)
 * Descripcion  : Cabecera que define la estructura del Elemento de
 *                Configuracion (CI), la lista doblemente enlazada que los
 *                administra en memoria, y los prototipos de todas las
 *                funciones publicas del modulo (CRUD, reportes y utilidades).
 * Estructura   : Se declara una lista doblemente enlazada NO circular, con
 *                punteros a cabeza y cola para insercion/recorrido en O(1)
 *                en los extremos, y punteros "anterior"/"siguiente" en cada
 *                nodo para permitir el recorrido bidireccional exigido por
 *                el requisito tecnico "uso intensivo de punteros".
 * ==========================================================================
 */

#ifndef CMDB_H
#define CMDB_H

/* Longitudes maximas para cada campo de texto del elemento de configuracion.
   Se definen como constantes simbolicas para evitar "numeros magicos" y
   facilitar el mantenimiento del codigo. */
#define TAM_CODIGO     15
#define TAM_NOMBRE     60
#define TAM_TIPO       30
#define TAM_VERSION    15
#define TAM_UBICACION  60
#define TAM_ESTADO     20
#define TAM_LINEA      300   // longitud maxima de una linea leida del archivo

/* --------------------------------------------------------------------------
 * Estructura: ElementoCI
 * Representa un Elemento de Configuracion dentro de la CMDB. Cada nodo
 * almacena los datos propios del elemento y dos punteros que lo enlazan
 * con el nodo anterior y el nodo siguiente de la lista.
 * ------------------------------------------------------------------------ */
typedef struct ElementoCI {
    char codigo[TAM_CODIGO];         // identificador unico del CI
    char nombre[TAM_NOMBRE];         // nombre descriptivo del CI
    char tipo[TAM_TIPO];             // categoria: Servidor, Aplicacion, Red, etc.
    char version[TAM_VERSION];       // version del elemento
    char ubicacion[TAM_UBICACION];   // ubicacion fisica o logica
    char estado[TAM_ESTADO];         // Activo, Inactivo, Mantenimiento, Retirado

    struct ElementoCI *anterior;     /* puntero al nodo previo (NULL si es
                                         la cabeza de la lista) */
    struct ElementoCI *siguiente;    /* puntero al nodo posterior (NULL si
                                         es la cola de la lista) */
} ElementoCI;

/* --------------------------------------------------------------------------
 * Estructura: ListaCMDB
 * Encapsula el estado global de la lista doblemente enlazada: puntero a la
 * cabeza, puntero a la cola y contador de elementos. Mantener la cola
 * permite insertar de forma ordenada sin recorrer toda la lista desde el
 * inicio cuando el nuevo codigo es mayor a todos los existentes.
 * ------------------------------------------------------------------------ */
typedef struct {
    ElementoCI *cabeza;
    ElementoCI *cola;
    int totalElementos;
} ListaCMDB;

/* ----------------------------- Ciclo de vida ---------------------------- */
void inicializarLista(ListaCMDB *lista);
void liberarLista(ListaCMDB *lista);

/* ------------------------------ Operaciones CRUD ------------------------ */
int registrarElemento(ListaCMDB *lista, ElementoCI datos);
ElementoCI *buscarPorCodigo(const ListaCMDB *lista, const char *codigo);
int modificarElemento(ListaCMDB *lista, const char *codigo, ElementoCI nuevosDatos);
int eliminarElemento(ListaCMDB *lista, const char *codigo);

/* ------------------------------ Reportes --------------------------------- */
void mostrarTodos(const ListaCMDB *lista);
int generarReportePorTipo(const ListaCMDB *lista, const char *tipo);

/* ------------------------------ Utilidades internas ----------------------
   No se documentan con el mismo nivel de detalle por ser auxiliares del
   propio modulo (no forman parte de la interfaz principal del menu). */
int existeCodigo(const ListaCMDB *lista, const char *codigo);                 // busqueda booleana
ElementoCI *crearNodo(ElementoCI datos);                                      // reserva memoria (malloc)
void insertarOrdenado(ListaCMDB *lista, ElementoCI *nodoNuevo);               // insercion por codigo

#endif /* CMDB_H */
