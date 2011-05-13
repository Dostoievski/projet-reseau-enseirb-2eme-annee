#ifndef FILTER
#define FILTER

/* #include <pcap.h> */
/* #include <stdio.h> */
/* #include <string.h> */
/* #include <stdlib.h> */
/* #include <ctype.h> */
/* #include <errno.h> */
/* #include <sys/types.h> */
/* #include <sys/socket.h> */
/* #include <netinet/in.h> */
/* #include <arpa/inet.h> */



#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/wait.h>


/* some protocoles*/
#define IPPROTO_ARP 1544
#define IPPROTO_RARP 13696

/*usual application protocoles*/
#define  HTTP 1
#define  FTP  2
#define  SSH  3
#define  IMAP 4
#define  POP3 5
#define  HTTPS  6
#define  TELNET 0
#define  ECHO   7
#define  DNS   8

#define  PROTOCOLES_NUM 9
/* a tab to strore usual protocoles stats*/
int protocoles[PROTOCOLES_NUM]; 


/* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define SIZE_ETHERNET 14

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

/* Ethernet header */
struct sniff_ethernet {
        u_char  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        u_char  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        u_short ether_type;                     /* IP? ARP? RARP? etc */
};

/* IP header */
struct sniff_ip {
        u_char  ip_vhl;                 /* version << 4 | header length >> 2 */
        u_char  ip_tos;                 /* type of service */
        u_short ip_len;                 /* total length */
        u_short ip_id;                  /* identification */
        u_short ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        u_char  ip_ttl;                 /* time to live */
        u_char  ip_p;                   /* protocol */
        u_short ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
};
#define IP_HL(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef u_int tcp_seq;

struct sniff_tcp {
        u_short th_sport;               /* source port */
        u_short th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        u_char  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        u_char  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        u_short th_win;                 /* window */
        u_short th_sum;                 /* checksum */
        u_short th_urp;                 /* urgent pointer */
};

/* prints protocoles stats*/
void count_protocoles_stats();

/*strores protocoles occurence stats*/
void
table_init ( int *, int);

/*increments protocoles occurences*/
void count_protocoles_inc(int);

/* to print packet paylods*/
void
print_payload(const u_char *payload, int len);

/* to print payload in ascii*/
void
print_hex_ascii_line(const u_char *payload, int len, int offset);

/*speaks for itself*/
void
print_app_usage(void);

/*verifie si text contient un mot de passe en clair */
void look_for_pswd (char * text,char * delimiter_);

#endif
