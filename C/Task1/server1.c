#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void handle_client(int client_sock) {
    int increment = 1;
    int value = 0;
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (bytes_received <= 0) break;

        if (buffer[0] == '+') {
            increment = atoi(buffer + 2);
            send(client_sock, "Ok", 2, 0);
        } else if (buffer[0] == '\\') {
            break;
        } else if (buffer[0] == '?') {
            sprintf(buffer, "%d", increment);
            send(client_sock, buffer, strlen(buffer), 0);
        } else {
            int num = atoi(buffer);
            value = num + increment;
            //value += atoi(buffer);
	    sprintf(buffer, "%d", value);
            send(client_sock, buffer, strlen(buffer), 0);
        }
    }

    close(client_sock);
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        if (fork() == 0) {
            close(server_sock);
            handle_client(client_sock);
        }
        close(client_sock);
    }

    close(server_sock);
    return 0;
}

