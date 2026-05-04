from multiprocessing import Process, Array
import time, random

def sensor(data):
    while True:
        data[0] = random.randint(0, 100)  # CPU
        data[1] = random.randint(0, 100)  # RAM
        time.sleep(1)

def dashboard(data):
    while True:
        print(f"CPU: {data[0]}% | RAM: {data[1]}%")
        time.sleep(1)

shared = Array('i', 2)

Process(target=sensor, args=(shared,), daemon=True).start()
Process(target=dashboard, args=(shared,), daemon=True).start()

time.sleep(10)
