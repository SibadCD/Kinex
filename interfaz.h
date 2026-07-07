/* ==========================================================================
 * Archivo      : interfaz.h
 * Descripcion  : Utilidades de presentacion en consola: codigos de color
 *                ANSI para diferenciar mensajes de exito/error/informacion,
 *                y funciones para pausar y limpiar la pantalla entre una
 *                accion del menu y la siguiente, sin afectar los datos ya
 *                guardados en memoria ni en el archivo de persistencia.
 * ==========================================================================
 */

#ifndef INTERFAZ_H
#define INTERFAZ_H

/* Codigos de escape ANSI. Funcionan de forma nativa en Linux y macOS, y
   en Windows 10 (build 1511) en adelante -tanto en cmd.exe/PowerShell
   como en la terminal integrada de VS Code- una vez habilitados con
   habilitarColoresWindows(). */
#define COLOR_RESET      "\x1b[0m"
#define COLOR_ROJO       "\x1b[31m"
#define COLOR_VERDE      "\x1b[32m"
#define COLOR_AMARILLO   "\x1b[33m"
#define COLOR_AZUL       "\x1b[34m"
#define COLOR_MAGENTA    "\x1b[35m"
#define COLOR_CIAN       "\x1b[36m"
#define COLOR_NEGRITA    "\x1b[1m"

void habilitarColoresWindows(void);
void limpiarPantalla(void);
void pausar(void);

/* Mensajes con formato tipo printf (admiten %s, %d, etc.) y color fijo
   segun su naturaleza, para que el usuario distinga de un vistazo si
   una accion tuvo exito, fallo, o es solo informativa. */
void imprimirExito(const char *formato, ...);
void imprimirError(const char *formato, ...);
void imprimirInfo(const char *formato, ...);

#endif /* INTERFAZ_H */
