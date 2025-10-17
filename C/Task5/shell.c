#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <unistd.h>
#include "list.h"
#include "tree.h"
#include "error.h"

// Глобальные переменные
pid_t bckrd_pid = 0;
int bckrd_flg = 0;
int bckrd_cmd = 0;

// Функция для вывода приглашения
void invitation(void) {
    char s[PATH_MAX];
    getcwd(s, PATH_MAX);
    //printf("%s", "\033[01;34m");
    printf("%s", s);
    //printf("%s", "\033[0m");
    printf(">>> ");
    fflush(stdout);
}

// Настройка обработки сигналов
void setup_signals() {
    signal(SIGINT, SIG_IGN);   // Игнорируем Ctrl+C в родительском процессе
    signal(SIGTTOU, SIG_IGN);  // Игнорируем сигналы управления терминалом
    signal(SIGTTIN, SIG_IGN);
    signal(SIGTSTP, SIG_IGN);  // Игнорируем Ctrl+Z
}
int max_count = 0;
int sum_pipes = 0;
int suma = 0;
void count_pipes(tree_node *root, int pipe_len) {
    if (root == NULL) {
        return;
    }

    if (root->pipe != NULL) {
	    suma = 1;
        count_pipes(root->pipe, pipe_len + 1);
    } else {
        if (pipe_len > max_count) {
            max_count = pipe_len;
        }
        pipe_len = 0;
	sum_pipes += suma;
	suma = 0;
    }

    count_pipes(root->next, pipe_len);
}

// Выполнение команды
int run_cmd(Node *list) {
    int err = 0;
    tree_node *root = NULL;
    printf("\n=== СПИСОК СОДЕРЖИТ ===\n"); // Добавить
    Node *tmp = list;
    while(tmp) {
        printf("Token: '%s'\n", tmp->data);
        fflush(stdout);
        tmp = tmp->next;
    }

    // Проверяем на ошибки синтаксиса
    err = check_error(list);
    if (!print_error(err)) {
        // Обрабатываем переменные окружения
        dollar_handler(list);

        // Подсчитываем количество фоновых команд
        bckrd_cmd = count_word(list, "&");

        // Строим дерево команд
        root = build_tree(list);
        // После строки root = build_tree(list);
        if (root == NULL) {
            detele_list(&list);
            return 0;
        }
	        count_pipes(root, 0);
if (max_count != 0) {
            printf("Максимальная длина подряд идущих пайпов: %d\n", max_count);
}
	printf("Сумма всех пайпов: %d\n", sum_pipes);
	// Выполняем команды
        err = tree_handler(root);

        // Освобождаем память
        delete_tree(root);
    }
    detele_list(&list);
    return err;
}

int main(int argc, char *argv[]) {
    Node *list = NULL;
    int err = 0;

    // Настраиваем обработку сигналов
    setup_signals();

    // Сохраняем копию стандартного ввода
    int stdin_copy = dup(0);

    while (!feof(stdin)) {
        // Выводим приглашение
        invitation();

        // Получаем список команд
        list = get_list();
        if (list == NULL) {
            continue;
        }

        // Проверяем на команду выхода
        if ((get_len(list) == 1) &&
            ((!strcmp(list->data, "exit")) || (!strcmp(list->data, "q")))) {
            detele_list(&list);
            break;
        }

        // Выполняем команды
        err = run_cmd(list);
        if (err == -1 || err == 1 || err == 13) {
           // printf("my_shell: incorrect command\n");
        }

        // Очищаем зомби-процессы
        int status;
        pid_t wpid;
        while ((wpid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED(status)) {
                printf("[finished]: %d, status=%d\n",
                       wpid, WEXITSTATUS(status));
            }
        }

        fflush(stdout);
    }

    // Восстанавливаем стандартный ввод
    if (stdin_copy != 0) {
        dup2(stdin_copy, 0);
        close(stdin_copy);
    }

    return 0;
}
