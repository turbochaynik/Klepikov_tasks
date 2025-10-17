#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <sys/time.h>

#define debug 1
#define BUFSIZE 256

typedef struct nodew_tag {
    char *word;
    struct nodew_tag *next;
} nodew;

/*Writes new gamer to GamerList and returnes his number.---------------------------*/
int AddNewGamer ();
/*Deletes gamer from GamerList.*/
void DeleteGamer (int gamer);
/*Disconnect and delete client from ClientList.*/
void DeleteClient (int number);

/*Sends a massage to the client with specified number.-----------------------------*/
/*If number == -1: sends a massage to everybody.*/
int SendMessage (int number, char *s);

/*Convert string to word list.-----------------------------------------------------*/
void String2Words (char *s, nodew **words);
/*Free memory from list of words.*/
void FreeWords (nodew **words);
/*Executes command in words. gamer - number of command sender. 
Returnes 1, if the game ends.*/
int Execute (int gamer, nodew *words);
/*Returnes pointer to word #num in word list or NULL.*/
char *Word (int num, nodew *words);

