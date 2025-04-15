// servidor.c - Adaptado com threads
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCK_PATH "/tmp/pipeso"
#define BUFFER_SIZE 1024

void *handle_client(void *arg) {//servidor receve 
    int client_sock = *(int *)arg;
    free(arg);
    char buffer[BUFFER_SIZE];

    ssize_t bytes_read = read(client_sock, buffer, sizeof(buffer));
    if (bytes_read > 0) {
        printf("[Thread] Requisição recebida: %s", buffer);

        
        for (int i = 0; i < strlen(buffer); i++) {// Processamento (exemplo: converter para maiúsculo)
            buffer[i] = toupper(buffer[i]);
        }

        write(client_sock, buffer, strlen(buffer) + 1);
        printf("[Thread] Resposta enviada.\n");
    } else {
        perror("Erro ao ler do cliente");
    }

    close(client_sock);
    pthread_exit(NULL);
}

int main() {
    int server_sock, *client_sock;
    struct sockaddr_un local, remote;
    socklen_t len;

    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Erro ao criar socket");
        exit(1);
    }

    memset(&local, 0, sizeof(local));
    local.sun_family = AF_UNIX;
    strncpy(local.sun_path, SOCK_PATH, sizeof(local.sun_path) - 1);
    unlink(SOCK_PATH);

    if (bind(server_sock, (struct sockaddr *)&local, sizeof(local)) < 0) {
        perror("Erro ao fazer bind");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Erro ao escutar");
        close(server_sock);
        exit(1);
    }

    printf("Servidor escutando em %s...\n", SOCK_PATH);

    while (1) {
        len = sizeof(remote);
        client_sock = malloc(sizeof(int));
        *client_sock = accept(server_sock, (struct sockaddr *)&remote, &len);
        if (*client_sock < 0) {
            perror("Erro ao aceitar conexão");
            free(client_sock);
            continue;
        }

        printf("Cliente conectado!\n");

        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, handle_client, client_sock) != 0) {
            perror("Erro ao criar thread");
            close(*client_sock);
            free(client_sock);
        } else {
            pthread_detach(thread_id); // Não precisa esperar a thread
        }
    }

    close(server_sock);
    return 0;
}
