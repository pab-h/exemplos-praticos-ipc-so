import socket
import threading

HOST = "localhost"
PORT = 9000

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

nome = input("Digite seu nome: ")
s.send(nome.encode())


def ouvir():
    while True:
        try:
            msg = s.recv(1024)
            if not msg:
                print("Conexão encerrada.")
                break
            print(msg.decode())
        except:
            print("Erro ao receber mensagem.")
            break


threading.Thread(target=ouvir, daemon=True).start()

while True:
    try:
        msg = input()
        if msg.lower() == "/sair":
            break
        s.send(msg.encode())
    except:
        break

s.close()
