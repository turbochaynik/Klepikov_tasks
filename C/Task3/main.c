#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "application.h"

#define N 4 // size of one block
#define SIZE 16

typedef struct
{
	char *data;
	size_t size;
	size_t capacity;
} Buf;

typedef struct
{
	char *data;
	size_t index;
	size_t size;
} Str;



char getsym(Str *str) // reading next char c from str
{
	if (str->index >= str->size) //if ended the str->data then we need to read more
	{
		str->index = 0;
		if (fgets(str->data, N, stdin) == NULL)
		{
			return EOF;
		}
		str->size = strlen(str->data);
	}
	char result = str->data[str->index];
	(str->index)++;
	return result;
}


void add_to_buf(Buf *buf, char c) // adding char c in the end of buf->data
{
	if (buf->size >= buf->capacity) // if memory of buf->data ended then we take more memory
	{
		buf->capacity += SIZE;
		buf->data = realloc(buf->data, buf->capacity);
	}
	buf->data[buf->size] = c;
	(buf->size)++;
}


int main() 
{

	Array lst;
        lst.words = malloc(sizeof(char*) * 16);
        lst.capacity = 16;
	lst.size = 0;

	size_t maxsize = 0;

	Str str;
	str.data = malloc(N);
	str.index = 0;
	str.size = 0;	

	Buf buf;
	buf.data = malloc(SIZE);
	buf.size = 0;
	buf.capacity = SIZE;

	char c;
	int flag = 1; //flag if eq 1 then we should read new value of c, else if eq 0 then we shouldn't read new value
	while(1)
	{
		if (flag) // we should read new value 
		{
			c = getsym(&str);
		}
		flag = 1;

		if (c == EOF) break;

		if (!isspace(c))
		{
			if (c == '|')
			{
				if (buf.size > 0) // because next if spec word
				{
					add_to_buf(&buf, '\0');
					add(&lst, buf.data);
					buf.size = 0;
				}
				c = getsym(&str); // symbol after first '|'
				if (c == '|') // if it is pair of '|', then it is full word
				{
					add_to_buf(&buf, '|');
					add_to_buf(&buf, '|');
					add_to_buf(&buf, '\0');
					add(&lst, buf.data);
					buf.size = 0;
				}
				else // if it is only one '|', then it is full word
				{
					add_to_buf(&buf, '|');
					add_to_buf(&buf, '\0');
					add(&lst, buf.data);
					buf.size = 0;
					flag = 0;
				}
			}	
			else if (c == '&')
			{
				if (buf.size > 0) // because next is spec word
				{
					add_to_buf(&buf, '\0');
                                	add(&lst, buf.data);
                                	buf.size = 0;
				}
                                c = getsym(&str); // symbol after first '&'
                                if (c == '&') // full word "&&"
                                {
                                        add_to_buf(&buf, '&');
                                        add_to_buf(&buf, '&');
                                        add_to_buf(&buf, '\0');
                                        add(&lst, buf.data);
                                        buf.size = 0;
                                }
                                else // ful word "&"
                                {
                                        add_to_buf(&buf, '&');
                                        add_to_buf(&buf, '\0');
                                        add(&lst, buf.data);
                                        buf.size = 0;
                                        flag = 0;
                                }
	
			}
			else if (c == '>')
			{
				if (buf.size > 0) // because next is special word
				{
					add_to_buf(&buf, '\0');
                                	add(&lst, buf.data);
                                	buf.size = 0;
				}
                                c = getsym(&str); // symbol after first '>'
                                if (c == '>') // full word ">>"
                                {
                                        add_to_buf(&buf, '>');
                                        add_to_buf(&buf, '>');
                                        add_to_buf(&buf, '\0');
                                        add(&lst, buf.data);
                                        buf.size = 0;
                                }
                                else // full word ">"
                                {
                                        add_to_buf(&buf, c);
                                        add_to_buf(&buf, '\0');
                                        add(&lst, buf.data);
                                        buf.size = 0;
                                        flag = 0;
                                }

			}
			else if ((c == ';') || (c == '<') || (c == '(') || (c == ')'))
			{
				if (buf.size > 0) //because next is special word
				{
					add_to_buf(&buf, '\0');
                                	add(&lst, buf.data);
                                	buf.size = 0;
				}
				add_to_buf(&buf, c);
                                add_to_buf(&buf, '\0');
                                add(&lst, buf.data);
                                buf.size = 0;

			}
			else add_to_buf(&buf, c); // not spec symbol
		}
		else 
		{
			if (buf.size > 0)
			{
				add_to_buf(&buf, '\0');
				add(&lst, buf.data);
				buf.size = 0;
			}
                        if ((c == '\n') && (lst.size > 0))
                        {
                                printf("\n\nLength: %zu\n\n", lst.size);
				print_list(&lst);
				printf("\n\nSorted:\n\n");
				sort(&lst);
				print_list(&lst);
				printf("\n");
				printf("other sort\n");
                		sort2(&lst);
                		print_list(&lst);
				if (maxsize < lst.size) maxsize = lst.size;
                                lst.size = 0;
				printf("\n\n\n\n");
                        }

		}
	}
	if (buf.size > 0)
	{
        	add_to_buf(&buf, '\0');
       		add(&lst, buf.data);
        	buf.size = 0;
        }
	if (lst.size > 0)
	{
		printf("\n\nLength: %zu\n\n", lst.size);
                print_list(&lst);
                printf("\n\nSorted:\n\n");
                sort(&lst);
                print_list(&lst);
		printf("other sort\n");
		sort2(&lst);
                print_list(&lst);
                if (maxsize < lst.size) maxsize = lst.size;
                lst.size = 0;
                printf("\n\n\n\n");

	}
	

	free(buf.data);
	free(str.data);
	lst.size = maxsize;
	free_list(&lst);
	return 0;
}
