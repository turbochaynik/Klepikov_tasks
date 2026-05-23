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
int server_running = 1;
pthread_mutex_t lock;
int server_sock;
pthread_t client_threads[MAX_CLIENTS];
int client_count = 0;
pthread_t input_thread;  // Поток для чтения команд с терминала

// Массив для хранения имён, которым отправлялись приватные сообщения
char *names[MAX_CLIENTS];
int inde = 0;

typedef struct {
    int socket;
    int index;
} ClientInfo;

void wakeup_accept() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock >= 0) {
        struct sockaddr_in dummy_addr;
        dummy_addr.sin_family = AF_INET;
        dummy_addr.sin_port = htons(PORT);
        dummy_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
        connect(sock, (struct sockaddr *)&dummy_addr, sizeof(dummy_addr));
        close(sock);
    }
}

void shutdown_server(int signum) {
    printf("Сервер завершает работу по сигналу %d...\n", signum);
    server_running = 0;
    shutdown(server_sock, SHUT_RDWR);
    close(server_sock);
    wakeup_accept();
}

void *handle_client(void *arg) {
    ClientInfo *ci = (ClientInfo *)arg;
    int client_sock = ci->socket;
    int index = ci->index;
    free(ci);

    char buffer[1024];
    // Получаем имя клиента (ожидается, что имя отправлено с разделителем '\n')
    int bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received <= 0) {
        close(client_sock);
        return NULL;
    }
    buffer[bytes_received] = '\0';
    char *nickname = strtok(buffer, "\n");
    if (nickname == NULL) {
        close(client_sock);
        return NULL;
    }
    pthread_mutex_lock(&lock);
    strncpy(clients[index].name, nickname, MAX_NAME_LEN - 1);
    clients[index].name[MAX_NAME_LEN - 1] = '\0';
    pthread_mutex_unlock(&lock);
    printf("Клиент зарегистрировался под именем: %s\n", clients[index].name);

    while (server_running) {
        memset(buffer, 0, sizeof(buffer));
        bytes_received = recv(client_sock, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0)
            break;
        buffer[bytes_received] = '\0';

        pthread_mutex_lock(&lock);
        if (buffer[0] == '+') {
            global_increment = atoi(buffer + 1);
            sprintf(buffer, "Global increment set to %d\n", global_increment);
            send(client_sock, buffer, strlen(buffer), 0);
        } else if (buffer[0] == '?') {
            sprintf(buffer, "Current global increment: %d\n", global_increment);
            send(client_sock, buffer, strlen(buffer), 0);
        } else if (buffer[0] == '\\') {
            pthread_mutex_unlock(&lock);
            break;
        } else {
            int num = atoi(buffer);
            stored_values[stored_count] = num;
            stored_sockets[stored_count] = client_sock;
            stored_count++;

            if (stored_count == 2) {
                int result = stored_values[0] + stored_values[1] + global_increment;
                sprintf(buffer, "Result: %d\n", result);
                send(stored_sockets[0], buffer, strlen(buffer), 0);
                send(stored_sockets[1], buffer, strlen(buffer), 0);
                stored_count = 0;
            } else {
                send(client_sock, "Waiting for another number...\n",
                     strlen("Waiting for another number...\n"), 0);
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
        if (fgets(command, sizeof(command), stdin) == NULL)
            break;
        if (strncmp(command, "help", 4) == 0) {
            printf("Доступные команды:\n");
            printf("private \"nickname\" \"message\" - приватное сообщение пользователю с именем <nickname>\n");
            printf("privates – получить список имен, которым отправлялись приватные сообщения\n");
            printf("exit - завершить работу сервера (если нет клиентов)\n");
        } else if (strncmp(command, "exit", 4) == 0) {
            printf("Сервер завершает работу...\n");
            shutdown_server(0);
            break;
        } else if (strncmp(command, "private ", 8) == 0) {
            char target_name[MAX_NAME_LEN], message[200];
            if (sscanf(command + 8, "\"%[^\"]\" \"%[^\"]\"", target_name, message) == 2) {
                int found = 0;
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
                        found = 1;
                        break;
                    }
                }
                if (!found) {
                    printf("Клиент с именем %s не найден\n", target_name);
                }
                fflush(stdout);
                pthread_mutex_unlock(&lock);
            } else {
                printf("Неверный формат. Используйте: private \"имя\" \"сообщение\"\n");
                fflush(stdout);
            }
        } else if (strncmp(command, "privates", 8) == 0) {
            pthread_mutex_lock(&lock);
            printf("Список клиентов, которым отправлялись сообщения:\n");
            for (int i = 0; i < inde; i++) {
                printf("%s\n", names[i]);
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

    signal(SIGINT, shutdown_server);
    signal(SIGTSTP, shutdown_server);
    signal(SIGTERM, shutdown_server);

    pthread_mutex_init(&lock, NULL);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);
    int opt = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock, MAX_CLIENTS) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен на порту %d\n", PORT);
    printf("Введите 'help'  в терминале сервера для просмотра команд.\n");

    pthread_create(&input_thread, NULL, server_input_listener, NULL);

    while (server_running) {
        addr_size = sizeof(client_addr);
        ClientInfo *ci = malloc(sizeof(ClientInfo));
        if (!ci) {
            perror("Malloc failed");
            continue;
        }
        ci->socket = accept(server_sock, (struct sockaddr *)&client_addr, &addr_size);
        if (!server_running) {
            free(ci);
            break;
        }
        if (ci->socket < 0) {
            free(ci);
            continue;
        }
        pthread_mutex_lock(&lock);
        if (client_count < MAX_CLIENTS) {
            ci->index = client_count;
            clients[client_count].socket = ci->socket;
            clients[client_count].name[0] = '\0';
            if (pthread_create(&client_threads[client_count], NULL, handle_client, ci) != 0) {
                perror("Failed to create client thread");
                close(ci->socket);
                free(ci);
            } else {
                client_count++;
            }
        } else {
            close(ci->socket);
            free(ci);
        }
        pthread_mutex_unlock(&lock);
    }

    pthread_cancel(input_thread);
    pthread_join(input_thread, NULL);

    for (int i = 0; i < client_count; i++) {
        pthread_join(client_threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    // Освобождаем память, выделенную для имен получателей приватных сообщений
    for (int i = 0; i < inde; i++) {
        free(names[i]);
    }

    return 0;
}

