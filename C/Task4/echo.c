#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ensure_capacity(char** buffer, size_t* capacity, size_t required_size) {
    if (required_size >= *capacity) {
        *capacity *= 2;
        *buffer = realloc(*buffer, *capacity);
        if (*buffer == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(EXIT_FAILURE);
        }
    }
}

void addword(const char* word, char** buffer, size_t* pos, size_t* capacity, int interpret_esc) {
    for (size_t i = 0; word[i] != '\0'; ++i) {
        if (interpret_esc && word[i] == '\\') {
            char next = word[i + 1];
            if (next == 'n') (*buffer)[(*pos)++] = '\n';
            else if (next == 't') (*buffer)[(*pos)++] = '\t';
            else if (next == 'r') (*buffer)[(*pos)++] = '\r';
            else if (next == 'a') (*buffer)[(*pos)++] = '\a';
            else if (next == 'b') (*buffer)[(*pos)++] = '\b';
            else if (next == 'f') (*buffer)[(*pos)++] = '\f';
            else if (next == 'v') (*buffer)[(*pos)++] = '\v';
            else if (next == '0') { // Octal
                int value = 0, k = 1;
                while (word[i + k] >= '0' && word[i + k] <= '7' && k <= 3) {
                    value = value * 8 + (word[i + k] - '0');
                    k++;
                }
                (*buffer)[(*pos)++] = (char)value;
                i += k - 1;
            } else if (next == 'x') { // Hexadecimal
                int value = 0, k = 2;
                while (word[i + k] && ((word[i + k] >= '0' && word[i + k] <= '9') ||
                                       (word[i + k] >= 'a' && word[i + k] <= 'f') ||
                                       (word[i + k] >= 'A' && word[i + k] <= 'F'))) {
                    char digit = word[i + k];
                    value = value * 16 + (digit <= '9' ? digit - '0' : (digit & ~32) - 'A' + 10);
                    k++;
                }
                (*buffer)[(*pos)++] = (char)value;
                i += k - 1;
            } else if (next == '\\') (*buffer)[(*pos)++] = '\\';
            else (*buffer)[(*pos)++] = next;
            i++;
        } else {
            (*buffer)[(*pos)++] = word[i];
        }
        ensure_capacity(buffer, capacity, *pos + 2);
    }
    (*buffer)[(*pos)++] = ' ';
    ensure_capacity(buffer, capacity, *pos + 2);
}

int main(int argc, char* argv[]) {
    int no_newline = 0;     
    int interpret_esc = 0; 
    size_t buffer_size = 128;
    char* output = calloc(buffer_size, sizeof(char));
    size_t pos = 0;

    if (output == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        return EXIT_FAILURE;
    }

    if (argc == 1) return 0;

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] != '\0') {
            // Обработка флагов
            for (int j = 1; argv[i][j] != '\0'; ++j) {
                if (argv[i][j] == 'n') no_newline = 1;
                else if (argv[i][j] == 'e') interpret_esc = 1;
                else if (argv[i][j] == 'E') interpret_esc = 0;
                else continue;
            }
            continue;
        }

        addword(argv[i], &output, &pos, &buffer_size, interpret_esc);
    }

    if (pos > 0 && output[pos - 1] == ' ') pos--; 
    if (!no_newline) output[pos++] = '\n';
    output[pos] = '\0';

    printf("%s", output);
    free(output);
    return 0;
}

