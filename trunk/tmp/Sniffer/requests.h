/*
 * requests.h
 * Xavier Delord
 * 12.11.2002
 */

#define MAXPATH 256

/* Client requests */
#define REQUEST_GET 1
#define REQUEST_LIST 2

/* Server answers */
#define ANSWER_OK      0
#define ANSWER_UNKNOWN 1        /* unknown request */
#define ANSWER_ERROR   2        /* error while processing request */

struct request {
    int  kind;
    char path[MAXPATH];
    
};

struct answer {
    int  ack;
    int  nbbytes;               /* for GET only */
    int  errnum;                /* significant if ack == ERROR and != 0 */
};

/* Port number deduced from UID allowing many users on a sigle host */
#define PORT (IPPORT_USERRESERVED + getuid())
