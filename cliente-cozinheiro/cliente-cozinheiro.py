from multiprocessing import Process, Queue
import time, random

def cliente(q, id):
    for i in range(3):
        pedido = f"Cliente {id} pediu item {i}"
        q.put(pedido)
        time.sleep(random.random())

def cozinheiro(q):
    while True:
        pedido = q.get()
        if pedido is None:
            break
        print("Preparando:", pedido)
        time.sleep(1)

q = Queue()

coz = Process(target=cozinheiro, args=(q,))
coz.start()

processos = []
for i in range(3):
    p = Process(target=cliente, args=(q, i))
    p.start()
    processos.append(p)

for p in processos:
    p.join()

q.put(None)  # encerra o cozinheiro
coz.join()
