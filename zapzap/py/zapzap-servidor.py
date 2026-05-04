import socket
import threading

HOST = "0.0.0.0"
PORT = 9000

clients = []
lock = threading.Lock()


def broadcast(msg, sender_conn):
    with lock:
        for client in clients:
            if client != sender_conn:
                try:
                    client.send(msg)
                except:
                    clients.remove(client)


def handle_client(conn, addr):
    print(f"[+] Conectado: {addr}")

    try:
        # Recebe nome do usuário
        nome = conn.recv(1024).decode()
        broadcast(f"[{nome} entrou no chat]".encode(), conn)

        while True:
            msg = conn.recv(1024)

            if not msg:
                break

            mensagem_formatada = f"{nome}: {msg.decode()}"
            print(mensagem_formatada)

            broadcast(mensagem_formatada.encode(), conn)

    except Exception as e:
        print(f"[ERRO] {addr}: {e}")

    finally:
        with lock:
            if conn in clients:
                clients.remove(conn)
        conn.close()
        print(f"[-] Desconectado: {addr}")


def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen()

    print(f"Servidor rodando em {HOST}:{PORT}")

    while True:
        conn, addr = server.accept()

        with lock:
            clients.append(conn)

        threading.Thread(
            target=handle_client,
            args=(conn, addr),
            daemon=True
        ).start()


if __name__ == "__main__":
    main()
