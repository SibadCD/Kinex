# ==========================================================================
# Makefile - Proyecto KINEX (Modulo CMDB - Grupo 6)
# Compila cada archivo .c por separado y los enlaza en un unico ejecutable.
# ==========================================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
OBJETOS = main.o cmdb.o persistencia.o validaciones.o interfaz.o
EJECUTABLE = kinex

all: $(EJECUTABLE)

$(EJECUTABLE): $(OBJETOS)
	$(CC) $(CFLAGS) -o $(EJECUTABLE) $(OBJETOS)

main.o: main.c cmdb.h persistencia.h validaciones.h interfaz.h
	$(CC) $(CFLAGS) -c main.c

cmdb.o: cmdb.c cmdb.h interfaz.h
	$(CC) $(CFLAGS) -c cmdb.c

persistencia.o: persistencia.c persistencia.h cmdb.h interfaz.h
	$(CC) $(CFLAGS) -c persistencia.c

validaciones.o: validaciones.c validaciones.h
	$(CC) $(CFLAGS) -c validaciones.c

interfaz.o: interfaz.c interfaz.h
	$(CC) $(CFLAGS) -c interfaz.c

clean:
	rm -f $(OBJETOS) $(EJECUTABLE)

.PHONY: all clean
