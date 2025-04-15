// cliente.c - versão Linux com pipe nomeado (FIFO)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define PIPE_NAME "pipe_grbd"

int main() {
    int fd;
    char buffer[BUFFER_SIZE];

    // Abre o pipe para escrita
    fd = open(PIPE_NAME, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe");
        exit(EXIT_FAILURE);
    }

    printf("Conectado ao servidor GRBD!\n");

    while (1) {
        printf("\nDigite a requisição (ou 'exit' para sair):\n> ");
        fgets(buffer, BUFFER_SIZE, stdin);

        // Remover o '\n' do final
        buffer[strcspn(buffer, "\n")] = 0;

        // Sair se o usuário digitar 'exit'
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Envia a requisição pelo pipe
        if (write(fd, buffer, strlen(buffer)) == -1) {
            perror("Erro ao escrever no pipe");
            close(fd);
            exit(EXIT_FAILURE);
        }

        printf("Requisição enviada ao servidor.\n");
    }

    close(fd);
    return 0;
}
