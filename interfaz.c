/* ==========================================================================
 * Archivo      : interfaz.c
 * Descripcion  : Implementacion de las utilidades de color, pausa y
 *                limpieza de pantalla usadas por main.c entre una accion
 *                del menu y la siguiente.
 *
 * Importante: limpiarPantalla() solo borra lo que se ve en la terminal.
 * La informacion de la CMDB sigue intacta en memoria (la lista
 * doblemente enlazada) y en disco (elementos.txt); limpiar la pantalla
 * no borra ni afecta esos datos en absoluto.
 * ==========================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "interfaz.h"

#ifdef _WIN32
#include <windows.h>
#endif

/* En Windows, el "modo virtual terminal" que interpreta los codigos de
   escape ANSI no siempre viene activado por defecto en versiones
   antiguas de la consola. Esta funcion lo activa; en Linux/macOS no
   hace nada (las terminales ya soportan ANSI de forma nativa). */
void habilitarColoresWindows(void) {
#ifdef _WIN32
    HANDLE salida = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo = 0;

    if (salida == INVALID_HANDLE_VALUE) return;
    if (!GetConsoleMode(salida, &modo)) return;

    modo |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(salida, modo);
#endif
}

/* system("cls"/"clear") depende del sistema operativo; se resuelve en
   tiempo de compilacion con _WIN32, que el compilador define
   automaticamente al construir para Windows. */
void limpiarPantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* Detiene la ejecucion hasta que el usuario presione ENTER, dandole
   tiempo de leer el resultado de la accion antes de limpiar la
   pantalla y volver a mostrar el menu. */
void pausar(void) {
    printf(COLOR_NEGRITA "\nPresione ENTER para continuar..." COLOR_RESET);
    fflush(stdout);
    getchar();
}

void imprimirExito(const char *formato, ...) {
    va_list argumentos;
    printf(COLOR_VERDE "[OK] ");
    va_start(argumentos, formato);
    vprintf(formato, argumentos);
    va_end(argumentos);
    printf(COLOR_RESET "\n");
}

void imprimirError(const char *formato, ...) {
    va_list argumentos;
    printf(COLOR_ROJO "[ERROR] ");
    va_start(argumentos, formato);
    vprintf(formato, argumentos);
    va_end(argumentos);
    printf(COLOR_RESET "\n");
}

void imprimirInfo(const char *formato, ...) {
    va_list argumentos;
    printf(COLOR_CIAN "[INFO] ");
    va_start(argumentos, formato);
    vprintf(formato, argumentos);
    va_end(argumentos);
    printf(COLOR_RESET "\n");
}
