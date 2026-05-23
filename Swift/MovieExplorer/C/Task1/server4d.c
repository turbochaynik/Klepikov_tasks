#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>

#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_NAME_LEN 50

typedef struct {
    int socket;
    char name[MAX_NAME_LEN];
} Client;

Client clients[MAX_CLIENTS];
int global_increment = 1;
int stored_values[2] = {0, 0};
int stored_sockets[2] = {-1, -1};
int stored_count = 0;
int client_count = 0;
pthread_mutex_t lock;
int server_sock;
int server_running = 1;
char *names[MAX_CLIENTS];
int inde = 0;

void *handle_client(void *arg) {
    int client_sock = *(int *)arg;
    char buffer[1024];
    char client_name[MAX_NAME_LEN] = {0};

    // Получаем имя клиента при подключении
    int bytes_received = recv(client_sock, client_name, MAX_NAME_LEN - 1, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return NULL;
    }
    client_name[bytes_received] = '\0';

    pthread_mutex_lock(&lock);
    if (client_count < MAX_CLIENTS) {
        clients[client_count].socket = client_sock;
        strncpy(clients[client_count].name, client_name, MAX_NAME_LEN);
        client_count++;
    }
    pthread_mutex_unlock(&lock);

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
    char command[256];

    while (server_running) {
        fgets(command, sizeof(command), stdin);
        if (strncmp(command, "help", 4) == 0) {
            printf("Доступные команды:\n");
            printf("private \"nickname\" \"message\" - приватное сообщение пользователю с именем <nickname>\n");
            printf("privates – получить от сервера имена пользователей, которым вы отправляли приватные сообщения\n");
            printf("exit - завершить работу сервера (если нет клиентов)\n");
        } else if (strncmp(command, "exit", 4) == 0) {
            printf("Сервер завершает работу...\n");
            fflush(stdout);
            server_running = 0;
            close(server_sock);  // Закрытие сокета перед завершением работы
        } else if (strncmp(command, "private ", 8) == 0) {
            char target_name[MAX_NAME_LEN], message[200];
            if (sscanf(command + 8, "\"%[^\"]\" \"%[^\"]\"", target_name, message) == 2) {
                pthread_mutex_lock(&lock);
                for (int i = 0; i < client_count; i++) {
                    if (strcmp(clients[i].name, target_name) == 0) {
                        int already_added = 0;
                        for (int j = 0; j < inde; j++) {
                            if (strcmp(names[j], target_name) == 0) {
                                already_added = 1;
                                break;
                            }
                        }
                        if (!already_added && inde < MAX_CLIENTS) {
                            names[inde] = strdup(target_name);
                            inde++;
                        }
                        send(clients[i].socket, message, strlen(message), 0);
                        printf("Сообщение отправлено клиенту %s\n", target_name);
                        fflush(stdout);
                        break;
                    }
                }
                pthread_mutex_unlock(&lock);
            } else {
                printf("Неверный формат. Используйте: private \"имя\" \"сообщение\"\n");
                fflush(stdout);
            }
        } else if (strncmp(command, "privates", 8) == 0) {
            pthread_mutex_lock(&lock);
            printf("Список клиентов, которым отправлялись сообщения:\n");
            for (int qw = 0; qw < inde; qw++) {
                printf("%s\n", names[qw]);
            }
            fflush(stdout);
            pthread_mutex_unlock(&lock);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    pthread_t input_thread;
    pthread_t client_threads[MAX_CLIENTS];

    pthread_mutex_init(&lock, NULL);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
    listen(server_sock, MAX_CLIENTS);

    printf("Сервер запущен на порту %d\n", PORT);
    printf("Введите 'help' для вывода всех допустимых команд \n");

    pthread_create(&input_thread, NULL, server_input_listener, NULL);

    while (server_running) {
        addr_size = sizeof(client_addr);
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (!server_running) break;  // Завершаем цикл после команды exit
        if (client_sock < 0) continue;

        pthread_create(&client_threads[client_count], NULL, handle_client, (void *)&client_sock);
    }

    // Ожидаем завершения всех потоков клиентов
    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    // Освобождение памяти для всех имен
    pthread_mutex_lock(&lock);
    for (int i = 0; i < inde; i++) {
        free(names[i]);
    }
    pthread_mutex_unlock(&lock);

    pthread_mutex_destroy(&lock);
    close(server_sock);  // Закрытие серверного сокета
    return 0;
}
