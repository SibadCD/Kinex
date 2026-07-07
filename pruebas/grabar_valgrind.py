import pexpect
import sys

entradas_file, log_file = sys.argv[1], sys.argv[2]
with open(entradas_file, "r", encoding="utf-8") as f:
    entradas = [l.rstrip("\n") for l in f if l.strip() != ""]

child = pexpect.spawn(
    "valgrind --leak-check=full --show-leak-kinds=all ./kinex",
    encoding="utf-8", timeout=8, dimensions=(200, 220)
)
child.logfile_read = open(log_file, "w", encoding="utf-8")

for entrada in entradas:
    while True:
        idx = child.expect(["Presione ENTER", ":\\s*$", pexpect.TIMEOUT], timeout=6)
        if idx == 0:
            child.sendline("")
            continue
        break
    child.sendline(entrada)

for _ in range(5):
    try:
        idx = child.expect(["Presione ENTER", pexpect.EOF, pexpect.TIMEOUT], timeout=8)
        if idx == 0:
            child.sendline("")
            continue
        break
    except Exception:
        break

try:
    child.expect(pexpect.EOF, timeout=10)
except Exception:
    pass

child.logfile_read.close()
print("Listo:", log_file)
