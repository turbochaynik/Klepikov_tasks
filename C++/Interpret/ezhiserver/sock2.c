#include "sock.h"
#include "time.h"
#define MAXSIZE 44

typedef struct Client_tag {
    int fd;
    int number;
    char *cmd;
    struct Client_tag *next;
} Client;
 
/*List of connected clients.*/
Client *ClientList;
/*Maximum client number.*/
int MaxCl;
/*Indicates, wether the game has started.*/
int GameStarted;

/*Reciving data from fd and proccessing it. Returns 1, if the game ends.*/
int Handle (int fd);
/*Convert command to word list and Execute received command.
Returns 1, if the game ends.*/
int Handle2 (Client *t);
/***********************************************************************************/
void PrintClientList ()
{
    Client *p;
    
    printf ("ClientList: {\n");
    p = ClientList;
    while (p != NULL) {
    	printf ("%d (%d)\n", p->number, p->fd);
    	p = p->next;
    }
    printf ("}\n");
}
/***********************************************************************************/

/*Open specified port for listening. 
Returnes listen socket descriptor or -1 if error occurs.*/
int OpenListenPort (int PortNum)
{
    int ls, tmp;
    struct sockaddr_in addr;

    if (PortNum < 0 || PortNum > 65535) {
    	printf ("A bad port number!\n");
    	return -1;
    }
    /*Creating socket.*/
    if ( -1 == (ls = socket (AF_INET, SOCK_STREAM, 0) )) {
    	printf ("Error: can't create socket!\n");    
	return -1;
    }
    /*Setting SO_REUSEADDR option for listening socket.*/
    tmp = 1;
    setsockopt (ls, SOL_SOCKET, SO_REUSEADDR, &tmp, sizeof(tmp));       
    /*Assigning a name to the socket.*/
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons (PortNum);
    if ( -1 == bind (ls, (struct sockaddr *)&addr, sizeof(addr)) ) {
    	printf ("Error: can't assign a name to the socket! Check port number!\n");
	return -1;
    }
    /*Setting listen mode for the socket.*/
    if ( -1 == listen (ls, 5)) {
    	printf ("Error: can't change port mode to listening!\n");
	return -1;
    }
    return ls;
}

/*Returnes number of clients in ClientList.*/
int ClientNumber ()
{
    Client *p;
    int number = 0;
    
    for (p = ClientList; p != NULL; p = p->next)
    	number++;
    return number;    
}

/*Accept new client from ls port. 
Returns new client's descriptor, -1 if error occurs.*/
int AcceptNewClient (int  ls)
{
    int fd;
    struct sockaddr_in addr;
    socklen_t len;
    Client **p;
    char s[BUFSIZE];
    
    /*Accepting new client.*/    
    len = sizeof (addr);    
    if ( -1 == (fd = accept (ls, (struct sockaddr *)&addr, &len)) ) {
    	printf ("Error: can't accept new client!\n");
	return -1;
    }    
    /*Checking clients limit.*/
    if (GameStarted) {
    	sprintf (s, "No more free places! Sorry!\n");
	write (fd, s, strlen (s)+1);
	shutdown (fd, 2);
	close (fd);
	return 0;
    }
#if debug
    printf ("A new client accepted: fd = %d, ip = %d, port = %d!\n", fd, addr.sin_addr.s_addr, addr.sin_port );    
#endif
    /*Writing information about new client to ClientList.*/
    p = &ClientList;
    while (*p != NULL)
    	p = &(*p)->next;
    *p = (Client *) malloc (sizeof (Client));
    (*p)->next = NULL;
    (*p)->fd = fd;
    (*p)->cmd = NULL;
    (*p)->number = AddNewGamer();
    sprintf (s, "Welcome! Your number is %d.\n", (*p)->number);
    SendMessage ((*p)->number, s);
    
#if debug
    PrintClientList ();
#endif
    if (ClientNumber () == MaxCl) {
    	GameStarted = 1;
    	SendMessage (-1, "Let's play!\n");
    }
    return fd;    
}

/*Disconnect and delete client from ClientList.*/
void DeleteClient (int number) 
{
    Client **pp, *t;
    
    pp = &ClientList;
    while (*pp != NULL && (*pp)->number != number)	
    	pp = &((*pp)->next);


    if (*pp != NULL) {
    	t = *pp;
    	DeleteGamer (t->number);
    	shutdown (t->fd, 2);
    	close (t->fd);
#if debug
    	printf ("Client %d disconnected!\n", t->number);	
#endif
	*pp = (*pp)->next;
    	free (t->cmd);
	free (t);    	    	    
#if debug
    	PrintClientList ();
#endif

    }
}

/*Setting connected clients' descriptors in descriptor set.*/
void SetClients (fd_set *fds)
{
    Client *p;
    p = ClientList;
    while (p != NULL) {
    	FD_SET (p->fd, fds);
	p = p->next;
    }
}


/***********************************************************************************/
int main (int argc, char *argv[])
{
    int ls, fd, maxfd;
    fd_set fds;
    Client *p;

    srand (time (0));
    GameStarted = 0;
    /*Making ClientList empty.*/
    ClientList = NULL;
    /*Opening specified port for listening.*/
    if (argc < 2) {
    	printf ("Please, specify listening port!\n");
	return 0;
    }
    /*Checking number of clients.*/
    if (argc < 3 || (MaxCl=atoi(argv[2]))<2) {
    	printf ("Please, specify right (>1) number of clients!\n");
	return 0;
    }
    if ( -1 == (maxfd = ls = OpenListenPort (atoi(argv[1]))) )
    	return 0;
    
    /*Multiplexing.*/
    for ( ;; ) {
    	FD_ZERO (&fds);
	FD_SET (ls, &fds);
    	SetClients (&fds);    	    
    	select (maxfd+1, &fds, NULL, NULL, NULL);
	if (FD_ISSET (ls, &fds)) {
	    fd = AcceptNewClient (ls);
	    if (fd > maxfd)
	    	maxfd = fd;
	}
    	p = ClientList;
    	while (p != NULL) {
    	    if (FD_ISSET (p->fd, &fds))
	    	if ( 1 == Handle (p->fd) )
		    return 0;
    	    p = p->next;
    	}
    }

    return 0;
}

/*Adds string src till '\n' or till '\0' to the end of string *dest.
Returns remained after '\n' string or NULL if there's no '\n' symbol.*/
char *Copy (char **dest, char *src)
{
    char *tmp;
    int destsize, srcsize, i;
        
    if (src == NULL)
    	return NULL; /*Nothing to add.*/
    /*Counting size of source and destination strings.*/
    if (*dest == NULL)
    	destsize = 0;
    else
    	destsize = strlen (*dest);
    for (srcsize=0; src[srcsize] != '\0' && 
    	    	    src[srcsize] != '\n' && src[srcsize] != '\r'; srcsize++);
       
    tmp = *dest;
    *dest = (char *) malloc (destsize+srcsize+1);
    if (tmp != NULL)
    	strcpy (*dest, tmp);
    for (i = 0; i < srcsize; i++)
    	(*dest)[destsize+i] = src[i];
    (*dest)[destsize+i] = '\0';
        
    if (tmp != NULL)
    	free (tmp);
    if (src[i] == '\r')
    	i++;
    if (src[i] == '\0')
    	return NULL;
    else {
    	return &(src[i+1]);
    }
}

int Handle (int fd)
{
    char s[MAXSIZE], *tmp;
    Client *t;
    int size;
    
    if ( 0 != (size = read (fd, s, MAXSIZE-1)) ) {
    	s[size] = '\0';	
    	/*Searching for proper client.*/  	    	    
	t = ClientList;
	while (t != NULL && t->fd != fd)
	    t = t->next;
    	tmp = s;
    	do {
    	    while (*tmp == '\0' && tmp < &s[size-1])    	    
	    	tmp++;
    	    /*Copiing old string and add new.*/
    	    tmp = Copy (&(t->cmd), tmp);    	  
    	    if (tmp != NULL) {
    	    	if ( 1 == Handle2 (t))
	    	    return 1;    
    	    	t->cmd[0] = '\0';
	    }
	} while (tmp != NULL && tmp < &s[size-1]);
    }
    else {
    	/*Client disconnected - deleting from list.*/
    	for (t = ClientList; t != NULL && t->fd != fd; t = t->next);
    	DeleteClient (t->number);
    }
    return 0;    
}

/*Converts command to list of words received commands and Executes it.
Retrunes 1, if the game ends.*/
int Handle2 (Client *t)
{
    int i;
    char s[256];
    nodew *p;
#if debug
    printf ("Received from client %d: <%s>\n", t->number, t->cmd);
#endif

    if (!GameStarted) {
    	i = ClientNumber ();
    	sprintf (s, "%d players connected. Waiting for %d more players.\n", i, MaxCl-i);
    	SendMessage (t->number, s);
    }
    else {
	String2Words (t->cmd, &p);
    	if (p != NULL) {
    	    if ( 1 == Execute (t->number, p) )
	    	return 1;    		
	    FreeWords (&p);   
	}
    }
    return 0;
}

/*Sends a massage to the client with specified number.-----------------------------*/
/*If number == -1: sends a massage to everybody.*/
int SendMessage (int number, char *s)
{
    Client *p;
    int i;
    
    for (p = ClientList; p != NULL; p = p->next) {
    	if (p->number == number || number == -1) {
    	    i = write (p->fd, s, strlen(s)+1);
    	    if (i<strlen(s)+1)
    	    	return -1;	
	}
    }

    return 0;
}
