#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree {
    int key;
    struct tree *left, *right;
} tree;

tree* createTree(int digit) {
	tree* newtree = (tree*)calloc(1, sizeof(tree));
	newtree->key = digit;
	newtree->left = NULL;
	newtree->right = NULL;
	return newtree;
}
int chars_to_int(char *s) {
	int i = 0;
	int digit = 0;
	while (s[i] != '\n' && s[i] != '\0' && s[i] != EOF){
		digit = digit * 10 + (s[i]- '0');
		i++;
	}
	return digit;
}

int search(tree* head, int digit) {
	tree* start = head;
	while (start != NULL) {
		if (digit < start->key) {
			start = start->left;
		} else if (digit > start->key) {
                        start = start->right;
		} else {
			return 1;
		}
		}
		return 0;
	}

void addElem(tree **head, int digit) {
	tree* newtree = createTree(digit);
	if (*head == NULL) {
		*head = newtree;
		return;
	}
	tree *current = *head;
	while (1){
		if (digit < current->key){
			if (current->left != NULL) current = current->left;
			else{
				current->left = newtree;
				return;
			}
		}
		else if(digit > current->key){
			if (current->right != NULL) current = current->right;
			else {
				current->right = newtree;
				return;
			}
		}
	}		
}

void countSubtrees(tree* head, int* leftCount, int* rightCount) {
    if (head == NULL) return;
    if (head->left != NULL) {
        (*leftCount)++;
        countSubtrees(head->left, leftCount, rightCount);
    }
    if (head->right != NULL) {
        (*rightCount)++;
        countSubtrees(head->right, leftCount, rightCount);
    }
}


int findMin(tree *head) {
	while(head->left != NULL) head = head->left;
	return head->key;
}

void deleteElem(tree **head, int digit) {
	tree *current = *head;
	tree *prev = NULL;
	while(1) {
		if (digit < current->key) {
			prev = current;
			current = current->left;}
		if (digit > current->key) {
			prev = current;
			current = current->right;}
		if (digit == current->key) {
			if ((current->left == NULL) && (current->right == NULL)) {
				if (prev == NULL) {
					*head = NULL;
					free(current);
					return;
				}	
				 else {
					if (prev->key > digit) prev->left = NULL;
					else prev->right = NULL;
					free(current);
					return;} 
			}
			else if (current->left == NULL){
				if (prev == NULL){
					*head = current->right;
					free(current);
					return;	
				}
				else {
					if (prev->key > digit) prev->left = current->right;
					else prev->right = current->right;
					free(current);
					return;}
			}
			else if (current->right == NULL){
                                if (prev == NULL){
                                        *head = current->left;
                                        free(current);
					return;
                                }
                                else {
                                        if (prev->key > digit) prev->left = current->left;
                                        else prev->right = current->left;
                                        free(current);
					return;}

				}
			else {
				int new_key = findMin(current->right);
				current->key = new_key;
				deleteElem(&(current->right), new_key);	
			}
			

}

}
}


int main() {
char op;
char input[10];
int digit;
int leftCount = 0;
int rightCount = 0;
tree* head = NULL;
while (fgets(input, sizeof(input), stdin)) {
        op = input[0]; 
	digit = chars_to_int(&input[1]);
	if (op == '+') {
		if (search(head, digit) == 0) {
		addElem(&head, digit);
		}
	}
	        if (op == '?') {
                if (search(head, digit) == 0) {
                printf("%d нет\n", digit);}
		else printf("%d да\n", digit);
                }
	if (op == '-') {
		if (search(head, digit) == 1) {
		deleteElem(&head, digit);
		}
		}
}
countSubtrees(head, &leftCount, &rightCount);
                printf("number of left: %d\n", leftCount);
                printf("number of right: %d\n", rightCount);
}
