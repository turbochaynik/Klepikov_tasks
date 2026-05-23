#include "sock.h"

/*Returnes 1, if sym is devider.*/
int IsDevider (char sym)
{
    if (sym == ' ' || sym == '\t' || sym == '\r' || sym == '\n' || sym == '\0')
    	return 1;
    return 0;
}

void PrintWords (nodew *words)
{
    nodew *p;
    
    printf ("<");
    for (p = words; p != NULL; p = p->next)
    	printf ("%s|", p->word);
    printf (">\n");
}

/*Free memory from list of words.*/
void FreeWords (nodew **words)
{
    nodew *p;
    
    while (*words != NULL) {
    	p = (*words)->next;
    	free ((*words)->word);
	free (*words);
	*words = p;	
    }
    *words = NULL;
}

/*Convert string to word list.*/
void String2Words (char *s, nodew **words)
{
    int i = 0, length;
    nodew **p;
    
    p = words;
    *p = NULL;
    while (s[i] != '\0') {
    	length = 0;
    	while (!IsDevider (s[i+length]))    
	    length++;
    	if (length != 0) {
    	    *p = (nodew *) malloc (sizeof(nodew));
    	    (*p)->next = NULL;    	    	    
	    (*p)->word = (char *) malloc (length+1);
    	    strncpy ((*p)->word, &s[i], length);
	    (*p)->word[length] = '\0';
    	    p = &(*p)->next;

	    i += length;	    
	}
	else
	    i++;
    }
#if debug
/*    PrintWords (*words);*/
#endif
}

/*Returnes pointer to word #num in word list or NULL.*/
char *Word (int num, nodew *words)
{
    nodew *p;
    p = words;

    if (num < 1)
    	return NULL;
    while (p != NULL && --num)
    	p = p->next;
    if (p == NULL)    
    	return NULL;
    else
    	return p->word;
}
