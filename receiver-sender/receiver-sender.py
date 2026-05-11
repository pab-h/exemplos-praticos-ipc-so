import multiprocessing
import time

def receiver(queue):

    # Recebe mensagem do tipo 1
    tipo, texto = queue.get()

    print("\n[RECEIVER]")
    print(f"Mensagem recebida: {texto}")

def sender(queue):

    tipo = 1
    texto = "Olá! Mensagem enviada via fila!"

    time.sleep(1)

    # Envia mensagem
    queue.put((tipo, texto))

    print("\n[SENDER]")
    print(f"Mensagem enviada: {texto}")

if __name__ == "__main__":

    # Cria a fila de mensagens
    queue = multiprocessing.Queue()

    # Cria os processos
    processo_receiver = multiprocessing.Process(
        target=receiver,
        args=(queue,)
    )

    processo_sender = multiprocessing.Process(
        target=sender,
        args=(queue,)
    )

    # Inicia os processos
    processo_receiver.start()
    processo_sender.start()

    # Espera os processos terminarem
    processo_sender.join()
    processo_receiver.join()