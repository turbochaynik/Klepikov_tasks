#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int sock;
char client_name[50];

void *receive_messages(void *arg) {
    char buffer[BUFFER_SIZE];
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("\n[PRIVATE MESSAGE]: %s\nCommand: ", buffer);
            fflush(stdout);
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Usage: %s <server_ip> <server_port> <nickname>\n", argv[0]);
        return 1;
    }

    strncpy(client_name, argv[3], sizeof(client_name) - 1);

    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Socket creation failed");
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return 1;
    }

    if (connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        return 1;
    }

    send(sock, client_name, strlen(client_name), 0);

    pthread_t receive_thread;
    pthread_create(&receive_thread, NULL, receive_messages, NULL);

    printf("Соединение %s с сервером установлено. Введите команду:\n", client_name);
    
    while (1) {
        printf("Command: ");
        fgets(buffer, BUFFER_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (send(sock, buffer, strlen(buffer), 0) < 0) {
            perror("Send failed");
            break;
        }

        if (strcmp(buffer, "\\") == 0) break;

        memset(buffer, 0, BUFFER_SIZE);
        if (recv(sock, buffer, BUFFER_SIZE - 1, 0) < 0) {
            perror("Receive failed");
            break;
        }

        printf("Server response: %s\n", buffer);
    }

    close(sock);
    return 0;
}
