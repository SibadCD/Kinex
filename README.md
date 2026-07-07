# KINEX — Gestión de Configuración (CMDB)

**KINEX** — de "Kinético" (movimiento) + "Nexo" (conexión) — es el módulo de
**Gestión de Configuración (CMDB)** desarrollado para la actividad evaluativa
RC2 del curso **ISWZ1102 – Programación 1**, dentro del caso de estudio
*Sistema Integral de Gestión de Servicios TI* de TechSolutions S.A.

El nombre resume la idea central del proyecto: una CMDB no es un registro
estático, sino una estructura que se reacomoda y reconecta constantemente
cada vez que se registra, modifica o elimina un elemento — igual que los
punteros de una lista doblemente enlazada se reescriben en cada operación.

## Integrantes — Grupo 6
- Davis (Bryan) [Apellido]
- Andrés [Apellido]
- Nicolás [Apellido]

## Descripción

El programa administra los **Elementos de Configuración (CI)** de la empresa
(servidores, aplicaciones, bases de datos, redes, servicios, etc.) utilizando
una **lista doblemente enlazada ordenada por código**, con persistencia en un
archivo de texto plano (`elementos.txt`).

### Funcionalidades
1. Registrar elemento
2. Modificar información
3. Buscar elemento
4. Eliminar elemento
5. Mostrar todos los elementos
6. Generar reporte por tipo
7. Guardar cambios en archivo

### Experiencia en consola
- **Banner e identidad visual**: KINEX se presenta con un arte ASCII al iniciar y un menú donde cada opción tiene un color según su naturaleza (verde = crear/guardar, amarillo = modificar, azul = buscar, rojo = eliminar, cian = listar, magenta = reportes).
- **Colores ANSI**: mensajes de éxito en verde, errores en rojo, información en cian. Funciona en Linux, macOS y Windows 10+ (cmd, PowerShell y la terminal de VS Code).
- **Pausa + limpieza de pantalla**: al terminar cada acción, el programa espera a que presiones ENTER y luego limpia la terminal antes de mostrar el menú de nuevo. Esto es solo visual: la información sigue intacta en memoria y en `elementos.txt`.
- **Validaciones reforzadas**: además de que los campos no estén vacíos y el estado sea válido, se rechaza el carácter `;` en cualquier campo de texto (rompería el formato del archivo de persistencia) y el código/versión no pueden empezar con `-`.

### Estructura del proyecto

```
KINEX/
├── main.c              # Banner, menú principal e interacción con el usuario
├── cmdb.h / cmdb.c      # Estructura ElementoCI, ListaCMDB y operaciones CRUD
├── persistencia.h / .c  # Guardado y carga de datos en archivo de texto
├── validaciones.h / .c  # Lectura segura de cadenas y validaciones de entrada
├── interfaz.h / .c      # Colores ANSI, pausa y limpieza de pantalla
├── Makefile             # Compilación modular
├── elementos.txt        # Archivo de datos (se crea/actualiza en ejecución)
└── pruebas/             # Casos de prueba y script de grabación de sesiones
```

## Compilación y ejecución

```bash
make       # compila los .c por separado y enlaza el ejecutable
./kinex    # ejecuta el programa
make clean # elimina los .o y el ejecutable
```

En Windows (VS Code + gcc/MinGW), si no tienes `make`:
```bash
gcc -Wall -Wextra -std=c11 -o kinex main.c cmdb.c persistencia.c validaciones.c interfaz.c
.\kinex.exe
```

## Pruebas de memoria (opcional)

```bash
valgrind --leak-check=full --show-leak-kinds=all ./kinex
```

## Licencia académica

Proyecto desarrollado con fines académicos para la Universidad de las
Américas (UDLA), Quito, Ecuador.
