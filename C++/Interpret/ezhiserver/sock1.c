#include "sock.h"

typedef struct Request_tag {
    int quantity, price;
} Request;
typedef struct intList_tag {
    int month_rest;
    struct intList_tag *next;
} intList;

typedef struct Gamer_tag {
    int number;
    int stuff, produce, plants, balance;
    Request buy_stuff, sell_produce;
    intList *new_plants; /*List of building plants.*/
    int new_produce;
    int finished;
    struct Gamer_tag *next;    
} Gamer;

typedef struct MarketState_tag {
    int state;
    int stuff_num, stuff_price; 
    int produce_num, produce_price;
} MarketState;

/*Number of connected gamers.*/
int GamerNumber = 0;
/*List of connected gamers.*/
Gamer *GamerList = NULL;
/*Market state*/
MarketState Market = {0, 0, 0, 0, 0};

/*Helps gamer with command. If cmd == NULL prints all commands.*/
void Help (char *cmd, int gamer);
/*Sends information about gamer.*/
void Information (int gamer, nodew *words);
/*Forms requests.*/
void FormRequest (int gamer, nodew *words);
/*Chage market state.*/
void ChangeMarket ();
/*Searches for bankrupts and winners. Returnes 1, when the game ends.*/
int Results ();
/*Perform request.*/
void PerformRequests ();
/*Decrease balance of all gamers for costs.*/
void Costs ();


/***********************************************************************************/
/*Prints GamerList.*/
void PrintGamerList ()
{
    Gamer *p;
    intList *q;
    
    printf
    ("Number\tStuff\tProduce\tPlants\tBalance\tBuy\tSell\tNProd..\tNPlants\tFinished\n");
    for (p = GamerList; p != NULL; p = p->next) {
    	printf ("%d\t%d\t%d\t%d\t%d\t%d,%d\t%d,%d\t%d\t", p->number, p->stuff, 
	    	p->produce, p->plants, p->balance, 
		p->buy_stuff.quantity, p->buy_stuff.price,
		p->sell_produce.quantity, p->sell_produce.price,
		p->new_produce);
    	q = p->new_plants;
    	if (q == NULL)
    	    printf ("NULL");
	else
	    while (q != NULL) {
	    	printf ("%d->", q->month_rest);
		q = q->next;
	    }
    	printf ("\t%d\n", p->finished);
    }
    printf ( "Market: %d: %d-%d; %d-%d\n", Market.state, 
    	     Market.stuff_num, Market.stuff_price, Market.produce_num, Market.produce_price );
}

/*Returns pointer to node in GamerList with number gamer, NULL if there's no gamer
with this number.*/
Gamer *GetGamer (int gamer)
{
    Gamer *p;
    for (p = GamerList; p != NULL && p->number != gamer; p = p->next);
    return p;
}
/***********************************************************************************/

/*Writes new gamer to GamerList and returnes his number.---------------------------*/
int AddNewGamer ()
{
    int number;
    Gamer **p, InitGamer = {0, 4, 2, 2, 10000, {0,0}, {0,0}, NULL, 0, 0, NULL};

    for (p = &GamerList; p != NULL && *p != NULL; p = &(*p)->next);
    *p = (Gamer *) malloc (sizeof (Gamer));

    **p = InitGamer;
    /*Searching for the first free number.*/
    for (number = 1; GetGamer (number) != NULL; number++);
    (*p)->number = number;
    GamerNumber++;
#if debug
    PrintGamerList ();
#endif

    return number;
}

/*Deletes gamer from GamerList.----------------------------------------------------*/
void DeleteGamer (int gamer)
{
    Gamer **p, *q;
    intList *l, *tmp;
    char s[BUFSIZE];
    
    for (p = &GamerList; *p != NULL && (*p)->number != gamer; p = &(*p)->next);
    if (*p != NULL) {
    	q = *p;
    	*p = (*p)->next;
    	for (l = q->new_plants; l != NULL; free (tmp) ) {
    	    tmp = l;
    	    l = l->next;
    	}
	free (q);
    }
    GamerNumber--;
    sprintf (s, "Gamer %d lost!\n", gamer);
    SendMessage (-1, s);   
}

/*Executes command in words. gamer - number of command sender. 
Returnes 1, if the game ends.*/
int Execute (int gamer, nodew *words)
{
    Gamer *p;
    char s[BUFSIZE];
    
    if (Market.state == 0) /*Initialise market state with 3.*/
    	ChangeMarket ();
	
    /*inf2*/
    if ( !strcmp ("inf2", Word (1, words)) ) {
    	PrintGamerList ();    	    
    	return 0;
    }
    /*inf gamer_num*/
    if ( !strcmp ("inf", Word (1, words)) ) {
    	if (Word (2, words)==NULL || Word (3, words)!=NULL)
	    Help ("inf", gamer);
	else
	    Information (gamer, words);
    	return 0;
    }
    /*market*/
    if ( !strcmp ("market", Word (1, words)) ) {
    	if (Word (2, words)!=NULL)
	    Help ("market", gamer);
	else
	    Information (gamer, words);
    	return 0;
    }

    /*produce number*/
    if ( !strcmp ("produce", Word (1, words)) ) {
    	if (Word (2, words)==NULL || Word (3, words)!=NULL)
	    Help ("produce", gamer);
	else
	    FormRequest (gamer, words);
    	return 0;
    }
    /*newplant*/
    if ( !strcmp ("newplant", Word (1, words)) ) {
    	if (Word (2, words)!=NULL)
	    Help ("newplant", gamer);
	else
	    FormRequest (gamer, words);
    	return 0;
    }
    /*buy\sell number price*/
    if ( !strcmp ("buy", Word (1, words)) || !strcmp ("sell", Word (1, words)) ) {
    	if (Word (2, words)==NULL || Word (3, words)==NULL || Word (4, words)!=NULL)
	    Help (Word (1, words), gamer);
	else
	    FormRequest (gamer, words);
    	return 0;
    }
    /*end*/
    if ( !strcmp ("end", Word (1, words)) ) {
    	if ( Word (2, words) != NULL)
	    Help ("end", gamer);
	else {
    	    p = GetGamer (gamer);
	    p->finished = 1;
    	    for (p = GamerList; p != NULL && p->finished; p = p->next);
    	    /*If everybody finished.*/
	    if (p == NULL) {
    	    	PerformRequests ();
		Costs ();
		if ( 1 == Results () )
		    return 1;
    	    	ChangeMarket ();			    	
		printf ("\nNew month!\n");
		SendMessage (-1, "New month!\n");
     	    }
	}
    	return 0;
    }
    /*help*/
    if ( !strcmp ("help", Word (1, words))) {
    	if (Word (3, words) != NULL) {
	    sprintf (s, "Wrong parameters! Please, type 'help help' to get help on help command.\n");
    	    SendMessage (gamer, s);
    	    return 0;
	}
    	Help (Word (2, words), gamer);    
    	return 0;
    }
    /*Unknown command.*/
    SendMessage (gamer, "Unknown command! Type 'help' to get information, please.\n");    
    return 0;
}

/***********************************************************************************/
/*Tasks performed as the reaction for commands.*/
/***********************************************************************************/

/*Helps gamer with command. If cmd == NULL prints all commands.*/
void Help (char *cmd, int gamer)
{
    char s[BUFSIZE];
    FILE *fp;
    
    if ( NULL == (fp = fopen ("help.txt", "r")) ) {
    	SendMessage (gamer, "Help system isn't available! Pease check, wether file help.txt exists.\n");
    	return;
    }    
    while ( NULL != fgets (s, BUFSIZE, fp) ) {
    	if ( cmd == NULL || !strncmp (cmd, s, 3) )
    	    SendMessage (gamer, s);    
    }    

    fclose (fp);
}

/*Sends information about gamer or market.*/
void Information (int gamer, nodew *words)
{
    char s[256];
    Gamer *p;

    if ( !strcmp (Word (1, words), "inf")) {
    	p = GetGamer (atoi (Word (2, words)));
    	if (p == NULL)
    	    sprintf (s, "A bad gamer number!\n");
	else {
    	    sprintf (s, "Number\tStuff\tProduce\tPlants\tBalance\n%d\t%d\t%d\t%d\t%d\t\n", p->number, p->stuff, 
	    	p->produce, p->plants, p->balance);
    	}	    
    }
    if ( !strcmp (Word (1, words), "market")) {
    	sprintf (s, "level = %d: %d stuff for $%d minimum; %d produces for $%d maximum.\n%d players are in business.\n", 
	    	 Market.state, Market.stuff_num, Market.stuff_price, 
		 Market.produce_num, Market.produce_price, GamerNumber);
    }
    SendMessage (gamer, s);
}

/*Forms requests.*/
void FormRequest (int gamer, nodew *words)
{
    const int ProducePrice = 2000, PlantPrice = 2500;
    Gamer *p;
    intList **l;
    int num, price;

    p = GetGamer (gamer);        	    	
    /*produce k*/
    if ( !strcmp ("produce", Word (1, words)) ) {
    	if ( 0 > (num = atoi (Word (2, words))) ) {
	    SendMessage (gamer, "Check second parameter, please!\n");
	    return;
	}
    	if ( p->stuff<num || 
	     p->balance<num*ProducePrice+p->buy_stuff.quantity*p->buy_stuff.price|| 
	     p->plants<p->new_produce+num) {
	    SendMessage (gamer, "You don't have enough resources!\n");
    	    return;	    
	}
    	p->stuff -= num;
	p->balance -= num*ProducePrice;
	p->new_produce += num;    	        	    	
#if debug
    	printf ("%d spent $%d to produce %d units. His balance = %d.\n", 
	    	gamer,num*ProducePrice, num, p->balance);
#endif
    }
    /*newplant*/    
    if ( !strcmp ("newplant", Word (1, words)) ) {
    	if ( p->balance < PlantPrice+p->buy_stuff.quantity*p->buy_stuff.price) {
	    SendMessage (gamer, "You don't have enough resources!\n");

	    return;
	}
    	p->balance -= PlantPrice;
    	for (l = &p->new_plants; *l != NULL; l = &(*l)->next);
	*l = (intList *) malloc (sizeof (intList));
	(*l)->next = NULL;
	(*l)->month_rest = 5;
#if debug
    	printf ("%d spent $%d to begin building a new plant. His balance = %d.\n", 
	    	gamer, PlantPrice, p->balance);
#endif
    }        
    /*buy number price*/
    if ( !strcmp ("buy", Word (1, words)) ) {
    	if ( 0 >= (num = atoi (Word (2, words))) || 
	     0 >= (price = atoi (Word (3, words)))) {
	    SendMessage (gamer, "Check parameters, please!\n");
	    return;
	}
    	if ( p->balance < num*price) {
	    SendMessage (gamer, "You don't have enough resources!\n");
    	    return;	    
	}
    	if ( price < Market.stuff_price ) {
	    SendMessage (gamer, "A very low price!\n");
	    return;
	}
    	p->buy_stuff.quantity = num;
	p->buy_stuff.price = price;
    }
    /*sell number price*/
    if ( !strcmp ("sell", Word (1, words)) ) {
    	if ( 0 >= (num = atoi (Word (2, words))) || 
	     0 >= (price = atoi (Word (3, words)))) {
	    SendMessage (gamer, "Check parameters, please!\n");
	    return;
	}
    	if ( p->produce < num) {
	    SendMessage (gamer, "You don't have enough resources!\n");
    	    printf ("Gamer %d don't have %d produce (only %d).\n", gamer, num, p->produce);
	    return;	    
	}
    	if ( price > Market.produce_price ) {
	    SendMessage (gamer, "A very high price!\n");
	    return;
	}
    	p->sell_produce.quantity = num;
	p->sell_produce.price = price;
    }
    
}


/***********************************************************************************/
/*Tasks performed at the beginning of the new month and at the end of it.*/
/***********************************************************************************/

/*Chage market state.*/
void ChangeMarket ()
{
    int value, k;
    k = Market.state;
    
    if (k != 0) {
    	value = rand () % 12; /*value = 0..11*/    
    	if (value <= 3); /*Save market state.*/    
    	if (4 <= value && value <= 6) {
    	    if (k != 1)
    	    	k--;
    	    else
    	    	k += 1+value/5; /*+1 if k == 4; +2 if k = 5..6*/
    	}
    	if (7 <= value && value <= 9) {
    	    if (k != 5)
	    	k++;
	    else
    	    	k -= 1+value/8; /*-1 if k == 7; -2 if k = 8..9*/
    	}
    	if (value == 10) {/*5<->1, 4<->2, 3->1*/
    	    if (k != 3)
	    	k = 6-k;
    	    else
    	    	k = 1;
    	}
    	if (value == 11) { /*4<->1, 5<->2, 3->5*/
    	    if (k != 3)
	    	k = (k+3) % 6;
	    else
    	    	k = 5;
    	}
    }
    else
    	k = 3;
    
    Market.state = k;
    Market.stuff_num = ((k*5 + 5)*GamerNumber)/10;
    Market.produce_num = ((35 - k*5)*GamerNumber)/10;
    Market.stuff_price = 950 - 150*(k/3) - (150 - 50*(k/3))*k;
    Market.produce_price = 7000 - 500*k;
}

/*Perform request.*/
void PerformRequests ()
{
    const int PlantPrice = 2500;
    Gamer *p;
    intList **l, *tmp = NULL;
    int stuff, produce, price, count, res;
    char s[BUFSIZE];
    
#if debug
    PrintGamerList ();
#endif
    for (p = GamerList; p != NULL; p = p->next) {
    	/*Producing.*/
	p->produce += p->new_produce;
	p->new_produce = 0;
	/*Building plants.*/
    	for (l = &p->new_plants; *l != NULL; ) {
    	    if (tmp != NULL)
	    	free (tmp);
    	    ((*l)->month_rest)--;
    	    if (1 == (*l)->month_rest)
	    	p->balance -= PlantPrice;
    	    if (0 == (*l)->month_rest) {
	    	tmp = *l;
    	    	*l = (*l)->next;
    	    	p->plants++;
	    }
	    else {
	    	tmp = NULL;
    	    	l = &(*l)->next;
    	    }
	}
    }
    /*Buy stuff.*/
    /*Counting requid stuff number.*/
    count = 0;
    for (p = GamerList; p != NULL; p = p->next)
    	count += p->buy_stuff.quantity;
    if (Market.stuff_num > count)
    	stuff = count;
    else
    	stuff = Market.stuff_num;
    if (stuff > 0)
    	SendMessage (-1, "Stuff auction:\tgamer\tnumber\tprice\n");
    	
    while (stuff > 0) {
    	/*Searching for maximum price and counting.*/    
    	price = Market.stuff_price-1; 
	count = 0;
	for (p = GamerList; p != NULL; p = p->next) {
    	    if (p->buy_stuff.price > price) {
	    	price = p->buy_stuff.price;
		count = 0;
	    } else {
	    	if (p->buy_stuff.price == price)
		    count++;
    	    }	    		    	
	}
	/*Sortition and searching for the luckiest.*/
	count = rand () % (count + 1) + 1;
	for (p = GamerList; p != NULL && count != 0; )
	    if (p->buy_stuff.price == price) {
	    	count--;
		if (count != 0)
		    p = p->next;			
	    }
	    else
	    	p = p->next;
    	if (p != NULL) {
	    if (stuff > p->buy_stuff.quantity)
	    	res = p->buy_stuff.quantity;
	    else
	    	res = stuff;
	    stuff -= res;
	    p->stuff += res;
	    p->balance -= res*price;
    	    /*Sending information.*/      	    
	    sprintf (s, "bought\t\t%d\t%d\t%d\n", p->number, res, price);
	    SendMessage (-1, s);
    	    p->buy_stuff.price = p->buy_stuff.quantity = 0;
#if debug
    	    printf ("%d bought %d stuff for $%d. His balance = %d.\n", 
	    	    p->number, res, res*price, p->balance);
#endif
	}
    }
    /*Sell produce.*/
    /*Counting requid produce number.*/
    count = 0;
    for (p = GamerList; p != NULL; p = p->next)
    	count += p->sell_produce.quantity;
    if (Market.produce_num > count)
    	produce = count;
    else
    	produce = Market.produce_num;
    if (produce > 0)
        SendMessage (-1, "Produce auction:gamer\tnumber\tprice\n");
    while (produce > 0) {
    	/*Searching for minimum price and counting.*/    
    	price = Market.produce_price+1; 
	count = 0;
	for (p = GamerList; p != NULL; p = p->next) {
    	    if (p->sell_produce.price != 0 && p->sell_produce.price < price) {
	    	price = p->sell_produce.price;
		count = 0;
	    } else {
	    	if (p->sell_produce.price == price)
		    count++;
    	    }	    		    	
	}
	/*Sortition and searching for the luckiest.*/
	count = rand () % (count + 1) + 1;
	for (p = GamerList; p != NULL && count != 0; )
	    if (p->sell_produce.price == price) {
	    	count--;
		if (count != 0)
			p = p->next;
	    }	
	    else
	    	p = p->next;
    	if (p != NULL) {
	    if (produce > p->sell_produce.quantity)
	    	res = p->sell_produce.quantity;
	    else
	    	res = produce;
	    produce -= res;
	    p->produce -= res;
	    p->balance += res*price;    	    
    	    /*Sending information.*/      	    
	    sprintf (s, "sold\t\t%d\t%d\t%d\n", p->number, res, price);
	    SendMessage (-1, s);
    	    p->sell_produce.price = p->sell_produce.quantity = 0;
#if debug
    	    printf ("%d sold %d produce for $%d. His balance = %d.\n", 
	    	    p->number, res, res*price, p->balance);
#endif
	}	    
    }
    /*Making requests zero.*/
    for (p = GamerList; p != NULL; p = p->next) {
    	p->buy_stuff.price = p->buy_stuff.quantity = 0;
	p->sell_produce.price = p->sell_produce.quantity = 0;
    	p->finished = 0;	    
    }
#if debug
    PrintGamerList ();
#endif

}

/*Decrease balance of all gamers for costs.*/
void Costs ()
{
    const int PlantCost = 1000, ProduceCost = 500, StuffCost = 300;
    Gamer *p;

    for (p = GamerList; p != NULL; p = p->next)
    	p->balance -= StuffCost*p->stuff + ProduceCost*p->produce + 
	    	      PlantCost*p->plants;
}

/*Searches for bankrupts and winners. Returnes 1, when the game ends.*/
int Results ()
{
    Gamer *p;
    
    for (p = GamerList; p != NULL; p = p->next) {
    	if (p->balance <= 0) {
	    SendMessage (p->number, "You are out of business!\n");
    	    printf ("Gamer %d got bankrupt!\n", p->number);
	    DeleteClient (p->number);
	}   	
    }

    if (GamerNumber == 1) {
    	SendMessage (GamerList->number, "Congratulations! You are a winner!\n");
    	return 1;
    }
    if (GamerNumber == 0)
    	return 1;
    return 0;
}
/***********************************************************************************/

