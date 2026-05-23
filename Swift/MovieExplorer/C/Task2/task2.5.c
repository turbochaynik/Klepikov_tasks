#include <stdio.h>

#include <stdlib.h>
#include <string.h>
// Структура, хранящая в себе слово и ссылку на следующий элемент
typedef struct elem {
    char *word;
    struct elem *next;
} elem;

// Функция для сравнения двух строк посимвольно пока обе строки не дойдут до \0
int compare(char *str1, char *str2) {
    while (*str1 && *str2) {
        if (*str1 != *str2) {
            return 0;
        }
        str1++;
        str2++;
    }
    return (*str1 == '\0' && *str2 == '\0');
}

// Функция, создающая новый элемент типа стуктура в списке
void addElem(elem **start, char *word) {
     elem *newElem = calloc(1, sizeof(elem)); // Так называемый "новый элемент"
    newElem->word = calloc(strlen(word) + 1, sizeof(char)); // Место, куда будет записано слово из буфера
    for (int i = 0; i <= strlen(word); i++) {
        newElem->word[i] = word[i];
    }
    newElem->next = NULL;
    if (*start == NULL) {
        *start = newElem;
    } else {
        elem *temp = *start;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newElem;
    }
}

int main() {
    char input[1024];
    printf("Введите строку: ");
    fgets(input, sizeof(input), stdin);
    elem *start = NULL; // Список слов
    int i = 0, j = 0;
    char word[100]; // Буфер для текущего слова
    while (input[i] != '\0') {
        if (input[i] != ' ' && input[i] != '\t' && input[i] != '\n') {
            word[j++] = input[i];
        } else if (j > 0) {
            word[j] = '\0';
            addElem(&start, word); // Функция для добавления нового слова в список
            j = 0;
        }
        i++;
    }
    elem *last = start;
    while (last->next != NULL) {
        last = last->next; // Последний элемент списка
    }
    char *lastWord = last->word; //Последнее слово списка
    elem *prev = NULL; // Элемент списка перед удаленным
    elem *current = start; // Иду по списку от начала к концу
    while ((current != NULL) && (current != last)) {
        if (compare(current->word, lastWord)) {
            elem *toDelete = current; //элемент списка, который совпал с последним
            if (prev == NULL) {
                start = current->next; //При удалении элемента связываем элементы вокруг удаленного
            } else {
                prev->next = current->next;
            }
            current = current->next;
            free(toDelete->word);
            free(toDelete);
        } else {
            prev = current;
            current = current->next;
        }
    }
int maxl = 10000;
int maxlen = 1024;
int massi[1000];
int q = 1;
elem* cur = start;
massi[0] = 1000;
int finder = 0;
    while (cur != NULL) {
	int leen = strlen(cur->word);
       // printf("%s", start->word);
	if (leen < maxl) {
		massi[q] = leen;
		maxl = leen;
		q+=1; }
	else if (leen > maxl) {
		for (finder = 0; finder <= q; finder++) {
			if (massi[q-finder] < leen) {
				massi[q-finder+1] = massi[q-finder]; }
			else { massi[q-finder+1] = leen; 
			break;}
		}
	q+=1;

	}
	


       /* if (start->next != NULL) {
            printf("s ");
        }*/
        cur = cur->next;
    }
for (finder = 1; finder <= q; finder++) {
	elem *cur = start;
	while (cur != NULL) {
		if (strlen(cur->word) == massi[finder]) {	
			printf("%s   ", cur->word);
			}	
	cur = cur->next;
		}	
}
    printf("\n");
        while (start != NULL) {
        elem *current = start;
        start = start->next;
        free(current->word);
        free(current);
    }
    return 0;
}

