import time
from multiprocessing import Process, Pipe

def filtro(conn):
    while True:
        msg = conn.recv()
        
        if msg is None:
            break
        
        if "ERROR" in msg:
            print("Filtrado:", msg)
    
    conn.close()

parent, child = Pipe()

p = Process(target=filtro, args=(child,))
p.start()

logs = [
    "INFO Sistema OK",
    "ERROR Falha no disco",
    "INFO Reiniciando",
    "ERROR Timeout"
]

for log in logs:
    parent.send(log)
    time.sleep(1)

parent.send(None)
parent.close()
p.join()
