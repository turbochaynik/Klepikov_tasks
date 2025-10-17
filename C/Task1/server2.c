#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10

int global_increment = 1;
int stored_values[2] = {0, 0};
int stored_sockets[2] = {-1, -1};
int stored_count = 0;
int server_running = 1;
pthread_mutex_t lock;
int server_sock;
pthread_t client_threads[MAX_CLIENTS];
int client_count = 0;

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[1024];

    while (server_running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) break;

        buffer[bytes_received] = '\0';

        pthread_mutex_lock(&lock);
        if (buffer[0] == '+') {
            global_increment = atoi(buffer + 2);
            send(client_sock, "Ok", 2, 0);
        } else if (buffer[0] == '?') {
            sprintf(buffer, "%d", global_increment);
            send(client_sock, buffer, strlen(buffer), 0);
        } else if (buffer[0] == '\\') {
            pthread_mutex_unlock(&lock);
            break;
        } else {
            int num = atoi(buffer);
            stored_values[stored_count] = num;
            stored_sockets[stored_count] = client_sock;
	    //send(client_sock, "Ok", 2, 0);
            stored_count++;

            if ((stored_count == 2) && (stored_sockets[0] != stored_sockets[1])) {
                int result = stored_values[0] + stored_values[1] + global_increment;
                sprintf(buffer, "%d", result);
                send(stored_sockets[0], buffer, strlen(buffer), 0);
                send(stored_sockets[1], buffer, strlen(buffer), 0);
                stored_count = 0;
            } else if ((stored_count == 2) && (stored_sockets[0] == stored_sockets[1])) {
		 int result = stored_values[0] + stored_values[1] + global_increment;
                sprintf(buffer, "%d", result);
                send(stored_sockets[0], buffer, strlen(buffer), 0);
	   stored_count = 0;
		} else {
		send(client_sock, "Ok", 2, 0);
	}
        }
        pthread_mutex_unlock(&lock);
    }
    close(client_sock);
    return NULL;
}

void *server_input_listener(void *arg) {
    char command[100];
    while (server_running) {
        fgets(command, sizeof(command), stdin);
        if (strncmp(command, "exit", 4) == 0) {
            printf("Сервер завершает работу...\n");
            server_running = 0;
            close(server_sock);
            break;
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t input_thread;

    pthread_mutex_init(&lock, NULL);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    printf("Сервер запущен на порту %d\n", PORT);
    printf("Введите 'exit' в терминале сервера для завершения.\n");

    pthread_create(&input_thread, NULL, server_input_listener, NULL);

    while (server_running) {
        addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (!server_running) break;
        if (client_sock < 0) continue;
        
        if (client_count < MAX_CLIENTS) {
            pthread_create(&client_threads[client_count++], NULL, handle_client, (void *)&client_sock);
        }
    }

    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);
    close(server_sock);
    return 0;
}

