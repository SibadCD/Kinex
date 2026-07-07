#!/usr/bin/env python3
"""Ejecuta el ejecutable compilado dentro de una pseudo-terminal (pty) real,
para que la entrada del usuario se vea "escrita" en la salida exactamente
como ocurriria en una sesion interactiva de verdad (con eco de terminal).

Maneja automaticamente el prompt "Presione ENTER para continuar..." que
aparece despues de cada accion del menu: cuando lo detecta, envia un ENTER
por su cuenta SIN consumir una linea del archivo de entradas, para que el
archivo de entradas no tenga que preveer manualmente cada pausa."""

import pexpect
import sys

entradas_file, log_file = sys.argv[1], sys.argv[2]

with open(entradas_file, "r", encoding="utf-8") as f:
    entradas = [linea.rstrip("\n") for linea in f if linea.strip() != ""]

child = pexpect.spawn("./kinex", encoding="utf-8", timeout=5, dimensions=(200, 220))
child.logfile_read = open(log_file, "w", encoding="utf-8")

PATRON_PAUSA = "Presione ENTER"
PATRON_PROMPT = ":\\s*$"

for entrada in entradas:
    while True:
        indice = child.expect([PATRON_PAUSA, PATRON_PROMPT, pexpect.TIMEOUT], timeout=3)
        if indice == 0:
            child.sendline("")   # responde la pausa por su cuenta
            continue             # sigue esperando el verdadero prompt
        break
    child.sendline(entrada)

# Consumir cualquier pausa final pendiente y dejar que el programa termine.
for _ in range(5):
    try:
        indice = child.expect([PATRON_PAUSA, pexpect.EOF, pexpect.TIMEOUT], timeout=3)
        if indice == 0:
            child.sendline("")
            continue
        break
    except Exception:
        break

child.logfile_read.close()
print("Sesion grabada en", log_file)
