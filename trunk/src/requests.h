#ifndef _REQUESTS_
#define _REQUESTS_

#define MAXPATH 256
#define MAXCLIENTS 30
/* Client requests */
#define REQUEST_GET 1
#define REQUEST_DIR 2
#define REQUEST_SNIFF 3

/* Server answers */
#define ANSWER_OK      0
#define ANSWER_UNKNOWN 1        /* unknown request */
#define ANSWER_ERROR   2        /* error while processing request */

struct request {
  int  kind;
  char path[MAXPATH]; /*for file path(get) and protocole name (sniff)also */
};

struct answer {
    int  ack;
    int  nbbytes;               /* for GET and sniff */
    int  errnum;                /* significant if ack == ERROR and != 0 */
};

/* Port number deduced from UID allowing many users on a sigle host */
//#define PORT (IPPORT_USERRESERVED + getuid())
#define PORT 8585

/* a table that contains client Ids depending on the protocole they asked for*/
int IP   [MAXCLIENTS];
int UDP  [MAXCLIENTS];
int TCP  [MAXCLIENTS];
int ICMP [MAXCLIENTS];
int RARP [MAXCLIENTS];
int ARP  [MAXCLIENTS];
int IGMP  [MAXCLIENTS];


/**********************************functions********************/



#endif
