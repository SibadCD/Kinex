/* ==========================================================================
 * Archivo      : main.c
 * Proyecto     : KINEX - Sistema de Gestion de Configuracion (CMDB)
 *                Desarrollado para: Sistema Integral de Gestion de Servicios
 *                TI - TechSolutions S.A.
 * Modulo       : Grupo 6 - Gestion de Configuracion (CMDB)
 * Curso        : ISWZ1102 - Programacion 1
 * Descripcion  : Punto de entrada del programa. Presenta el menu de opciones,
 *                captura y valida los datos del usuario, y delega toda la
 *                logica de negocio a los modulos cmdb.c (estructura de datos)
 *                y persistencia.c (archivo de texto).
 *
 * Sobre el nombre KINEX: mezcla de "Kinetico" (movimiento) y "Nexo"
 * (conexion), en referencia a que la CMDB no es un registro estatico:
 * cada alta, baja o modificacion reacomoda en el momento los enlaces
 * (punteros anterior/siguiente) entre los elementos de configuracion.
 *
 * Experiencia de uso: tras cada accion, el programa espera a que el
 * usuario presione ENTER (para darle tiempo de leer el resultado) y luego
 * limpia la pantalla antes de volver a mostrar el menu. Esto es solo
 * visual: los datos siguen intactos en memoria y en el archivo
 * elementos.txt: ver interfaz.c para el detalle de limpiarPantalla().
 * ==========================================================================
 */

#include <stdio.h>
#include <string.h>
#include "cmdb.h"
#include "persistencia.h"
#include "validaciones.h"
#include "interfaz.h"

#define ARCHIVO_DATOS "elementos.txt"

/* Prototipos de las funciones que implementan cada opcion del menu.
   Se mantienen en este archivo porque son exclusivas de la interfaz de
   usuario (no pertenecen a la logica pura del modulo CMDB). */
static void mostrarBienvenida(int registrosCargados);
static void menuRegistrar(ListaCMDB *lista);
static void menuModificar(ListaCMDB *lista);
static void menuBuscar(const ListaCMDB *lista);
static void menuEliminar(ListaCMDB *lista);
static void menuReportePorTipo(const ListaCMDB *lista);
static void mostrarMenu(void);
static ElementoCI solicitarDatosElemento(int pedirCodigo);

/* Imprime el banner de bienvenida con el nombre KINEX en ASCII art y el
   numero de elementos recuperados del archivo de persistencia. */
static void mostrarBienvenida(int registrosCargados) {
    printf(COLOR_CIAN " _  _____ _   _ _______  __" COLOR_RESET "\n");
    printf(COLOR_CIAN "| |/ /_ _| \\ | | ____\\ \\/ /" COLOR_RESET "\n");
    printf(COLOR_CIAN "| ' / | ||  \\| |  _|  \\  / " COLOR_RESET "\n");
    printf(COLOR_CIAN "| . \\ | || |\\  | |___ /  \\ " COLOR_RESET "\n");
    printf(COLOR_CIAN "|_|\\_\\___|_| \\_|_____/_/\\_\\" COLOR_RESET "\n");
    printf(COLOR_NEGRITA "  Kinetico + Nexo" COLOR_RESET " | Gestion de Configuracion (CMDB) - Grupo 6\n");
    printf("---------------------------------------------------\n");
    printf("Elementos cargados desde '%s': %d\n", ARCHIVO_DATOS, registrosCargados);
}

int main(void) {
    habilitarColoresWindows();   // sin efecto en Linux/macOS; necesario en Windows

    ListaCMDB cmdb;
    inicializarLista(&cmdb);

    int registros = cargarDesdeArchivo(&cmdb, ARCHIVO_DATOS);
    limpiarPantalla();
    mostrarBienvenida(registros);

    int opcion;
    do {
        mostrarMenu();
        if (scanf("%d", &opcion) != 1) {
            limpiarBufferEntrada();
            opcion = -1;                 // valor invalido para forzar el "default"
            imprimirError("Ingrese un numero de opcion valido.");
            pausar();
            limpiarPantalla();
            continue;
        }
        limpiarBufferEntrada();          // descarta el '\n' pendiente tras scanf

        switch (opcion) {
            case 1: menuRegistrar(&cmdb); pausar(); limpiarPantalla(); break;
            case 2: menuModificar(&cmdb); pausar(); limpiarPantalla(); break;
            case 3: menuBuscar(&cmdb); pausar(); limpiarPantalla(); break;
            case 4: menuEliminar(&cmdb); pausar(); limpiarPantalla(); break;
            case 5: mostrarTodos(&cmdb); pausar(); limpiarPantalla(); break;
            case 6: menuReportePorTipo(&cmdb); pausar(); limpiarPantalla(); break;
            case 7:
                if (guardarEnArchivo(&cmdb, ARCHIVO_DATOS)) {
                    imprimirExito("Datos guardados correctamente en '%s'.", ARCHIVO_DATOS);
                }
                pausar();
                limpiarPantalla();
                break;
            case 0:
                printf("\nGuardando cambios antes de salir...\n");
                guardarEnArchivo(&cmdb, ARCHIVO_DATOS);
                printf(COLOR_CIAN "Hasta pronto.\n" COLOR_RESET);
                break;
            default:
                imprimirError("Opcion no reconocida. Intente de nuevo.");
                pausar();
                limpiarPantalla();
        }

    } while (opcion != 0);

    liberarLista(&cmdb);   // libera toda la memoria dinamica antes de terminar
    return 0;
}

/* Imprime el menu principal en pantalla. Cada opcion tiene un color
   distinto segun el tipo de accion que representa, para que de un
   vistazo sea evidente cuales son "seguras" (verde), cuales requieren
   cuidado (amarillo/rojo) y cuales son solo de consulta (azul/cian):
     verde    -> crear/guardar (agrega o preserva informacion)
     amarillo -> modificar (cambia informacion existente)
     azul     -> buscar (consulta puntual, de solo lectura)
     rojo     -> eliminar (accion destructiva, la que mas cuidado requiere)
     cian     -> listar (consulta masiva, de solo lectura)
     magenta  -> reportes (analisis/filtrado especial) */
static void mostrarMenu(void) {
    printf("\n" COLOR_CIAN "===================================================\n");
    printf("   KINEX - Gestion de Configuracion (Grupo 6)\n");
    printf("===================================================" COLOR_RESET "\n");
    printf(COLOR_VERDE    " 1. Registrar elemento" COLOR_RESET "\n");
    printf(COLOR_AMARILLO " 2. Modificar informacion" COLOR_RESET "\n");
    printf(COLOR_AZUL     " 3. Buscar elemento" COLOR_RESET "\n");
    printf(COLOR_ROJO     " 4. Eliminar elemento" COLOR_RESET "\n");
    printf(COLOR_CIAN     " 5. Mostrar todos los elementos" COLOR_RESET "\n");
    printf(COLOR_MAGENTA  " 6. Generar reporte por tipo" COLOR_RESET "\n");
    printf(COLOR_VERDE    " 7. Guardar cambios en archivo" COLOR_RESET "\n");
    printf(COLOR_NEGRITA  " 0. Salir" COLOR_RESET "\n");
    printf(COLOR_CIAN "---------------------------------------------------" COLOR_RESET "\n");
    printf("Seleccione una opcion: ");
}

/* --------------------------------------------------------------------------
 * solicitarDatosElemento
 * Centraliza la captura por teclado de los seis campos de un CI, aplicando
 * las validaciones del modulo validaciones.c: campos obligatorios no
 * vacios, sin el caracter ';' (que rompe el formato del archivo de
 * persistencia), el codigo sin espacios ni signo negativo, la version sin
 * signo negativo, y el estado dentro del catalogo permitido. El parametro
 * "pedirCodigo" permite reutilizar esta funcion tanto para registrar
 * (codigo obligatorio) como para modificar (el codigo ya se conoce y no
 * se vuelve a pedir).
 * ------------------------------------------------------------------------ */
static ElementoCI solicitarDatosElemento(int pedirCodigo) {
    ElementoCI datos;
    memset(&datos, 0, sizeof(ElementoCI));

    if (pedirCodigo) {
        do {
            printf("Codigo del elemento (max %d caracteres): ", TAM_CODIGO - 1);
            leerCadena(datos.codigo, TAM_CODIGO);
            if (!codigoValido(datos.codigo)) {
                imprimirError("El codigo no puede estar vacio, contener ';', espacios ni empezar con '-'.");
            }
        } while (!codigoValido(datos.codigo));
    }

    do {
        printf("Nombre: ");
        leerCadena(datos.nombre, TAM_NOMBRE);
        if (!cadenaNoVacia(datos.nombre)) {
            imprimirError("El nombre no puede estar vacio.");
        } else if (!cadenaSegura(datos.nombre)) {
            imprimirError("El nombre no puede contener el caracter ';'.");
        }
    } while (!cadenaNoVacia(datos.nombre) || !cadenaSegura(datos.nombre));

    do {
        printf("Tipo (Servidor, Aplicacion, Base de Datos, Red, Servicio, etc.): ");
        leerCadena(datos.tipo, TAM_TIPO);
        if (!cadenaNoVacia(datos.tipo)) {
            imprimirError("El tipo no puede estar vacio.");
        } else if (!cadenaSegura(datos.tipo)) {
            imprimirError("El tipo no puede contener el caracter ';'.");
        }
    } while (!cadenaNoVacia(datos.tipo) || !cadenaSegura(datos.tipo));

    do {
        printf("Version: ");
        leerCadena(datos.version, TAM_VERSION);
        if (!numeroValido(datos.version)) {
            imprimirError("La version no puede estar vacia, contener ';' ni empezar con '-'.");
        }
    } while (!numeroValido(datos.version));

    do {
        printf("Ubicacion: ");
        leerCadena(datos.ubicacion, TAM_UBICACION);
        if (!cadenaNoVacia(datos.ubicacion)) {
            imprimirError("La ubicacion no puede estar vacia.");
        } else if (!cadenaSegura(datos.ubicacion)) {
            imprimirError("La ubicacion no puede contener el caracter ';'.");
        }
    } while (!cadenaNoVacia(datos.ubicacion) || !cadenaSegura(datos.ubicacion));

    do {
        printf("Estado (Activo / Inactivo / Mantenimiento / Retirado): ");
        leerCadena(datos.estado, TAM_ESTADO);
        if (!estadoValido(datos.estado)) {
            imprimirError("Estado no valido. Use uno de los cuatro estados indicados.");
        }
    } while (!estadoValido(datos.estado));

    return datos;
}

static void menuRegistrar(ListaCMDB *lista) {
    printf("\n" COLOR_VERDE "--- Registrar nuevo elemento de configuracion ---" COLOR_RESET "\n");
    ElementoCI datos = solicitarDatosElemento(1);

    int resultado = registrarElemento(lista, datos);
    if (resultado == 1) {
        imprimirExito("Elemento '%s' registrado correctamente.", datos.codigo);
    } else if (resultado == 0) {
        imprimirError("Ya existe un elemento con el codigo '%s'.", datos.codigo);
    } else {
        imprimirError("No hay memoria disponible para registrar el elemento.");
    }
}

static void menuModificar(ListaCMDB *lista) {
    char codigo[TAM_CODIGO];
    printf("\n" COLOR_AMARILLO "--- Modificar elemento existente ---" COLOR_RESET "\n");
    printf("Codigo del elemento a modificar: ");
    leerCadena(codigo, TAM_CODIGO);

    if (buscarPorCodigo(lista, codigo) == NULL) {
        imprimirError("No existe ningun elemento con el codigo '%s'.", codigo);
        return;
    }

    printf("Ingrese los nuevos datos (el codigo se mantiene igual):\n");
    ElementoCI nuevosDatos = solicitarDatosElemento(0);
    modificarElemento(lista, codigo, nuevosDatos);
    imprimirExito("Elemento '%s' actualizado correctamente.", codigo);
}

static void menuBuscar(const ListaCMDB *lista) {
    char codigo[TAM_CODIGO];
    printf("\n" COLOR_AZUL "--- Buscar elemento ---" COLOR_RESET "\n");
    printf("Codigo a buscar: ");
    leerCadena(codigo, TAM_CODIGO);

    ElementoCI *encontrado = buscarPorCodigo(lista, codigo);
    if (encontrado == NULL) {
        imprimirInfo("No se encontro ningun elemento con el codigo '%s'.", codigo);
        return;
    }

    printf("\n" COLOR_NEGRITA "Codigo     :" COLOR_RESET " %s\n", encontrado->codigo);
    printf(COLOR_NEGRITA "Nombre     :" COLOR_RESET " %s\n", encontrado->nombre);
    printf(COLOR_NEGRITA "Tipo       :" COLOR_RESET " %s\n", encontrado->tipo);
    printf(COLOR_NEGRITA "Version    :" COLOR_RESET " %s\n", encontrado->version);
    printf(COLOR_NEGRITA "Ubicacion  :" COLOR_RESET " %s\n", encontrado->ubicacion);
    printf(COLOR_NEGRITA "Estado     :" COLOR_RESET " %s\n", encontrado->estado);
}

static void menuEliminar(ListaCMDB *lista) {
    char codigo[TAM_CODIGO];
    printf("\n" COLOR_ROJO "--- Eliminar elemento ---" COLOR_RESET "\n");
    printf("Codigo del elemento a eliminar: ");
    leerCadena(codigo, TAM_CODIGO);

    if (eliminarElemento(lista, codigo)) {
        imprimirExito("Elemento '%s' eliminado correctamente.", codigo);
    } else {
        imprimirError("No existe ningun elemento con el codigo '%s'.", codigo);
    }
}

static void menuReportePorTipo(const ListaCMDB *lista) {
    char tipo[TAM_TIPO];
    printf("\n" COLOR_MAGENTA "--- Generar reporte por tipo ---" COLOR_RESET "\n");
    printf("Tipo a filtrar (ej. Servidor, Aplicacion, Red): ");
    leerCadena(tipo, TAM_TIPO);
    generarReportePorTipo(lista, tipo);
}
