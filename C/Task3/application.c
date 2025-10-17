#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "application.h"


void sort(Array *list) // sort the list 
{
	if (list->size <= 1) return; // if length of list 0 or 1, do nothing
	
	// bubble sort
	int swaps; // flag: 0 - already sorted, 1 - not sorted yet 
	for (size_t i = 0; i < list->size - 1; ++i) 
	{
		//swaps = 0;
		for (size_t j = 0; j < list->size - 1 - i; ++j)
		{
			if (strcmp(list->words[j], list->words[j + 1]) > 0)
			{
				swaps = 1;
				char *remember = list->words[j];
				list->words[j] = list->words[j + 1];
				list->words[j + 1] = remember;
			}
		}

		if (swaps == 0) break; // The list is sorted
	}
}

void sort2(Array *list) // sort the list
{
        if (list->size <= 1) return; // if length of list 0 or 1, do nothing

       // int swaps; // flag: 0 - already sorted, 1 - not sorted yet
        for (size_t i = 0; i < list->size - 1; ++i)
        {
               // swaps = 0;
                for (size_t j = 0; j < list->size - 1 - i; ++j)
                {
                        if ((strlen(list->words[j]) < strlen(list->words[j + 1])))
                        {
                               // swaps = 1;
                                char *remember = list->words[j];
                                list->words[j] = list->words[j + 1];
                                list->words[j + 1] = remember;
                        }
                }

                //if (swaps == 0) break; // The list is sorted
        }
}


void add(Array *list, char *word) // add string word in the end if list->words
{
	if(list->size == list->capacity) // no free memory of list->words
	{
		list->capacity *= 2;
		list->words = realloc(list->words, sizeof(char*) * list->capacity);
	}
	list->words[list->size] = malloc(strlen(word) + 1); // adding
	strcpy(list->words[list->size], word);
	++(list->size);
}


void free_list(Array *list) // free memory of list
{
	for (size_t i = 0; i < list->size; ++i) free(list->words[i]); // free every word in the list
	free(list->words);
}


void print_list(Array *list) // print elements of massive list->words
{
	for (size_t i = 0; i < list->size; ++i) printf("%s\n", list->words[i]);
}
