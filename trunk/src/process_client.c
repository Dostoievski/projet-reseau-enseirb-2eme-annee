#include "process_client.h"
#include "requests.h"


/*verifie si text contient un mot de passe en clair */
void look_for_pswd (char * text,char * delimiter_){
  char *str1 = malloc(strlen(text)*sizeof(char));
  
  char * delimiter = delimiter_;

  char *token;
  char *saveptr1,*saveptr2;
  

  for (strcpy(str1,text); ; str1 = NULL) {
    token = strtok_r(str1,delimiter, &saveptr1);
    if (token == NULL)
      break;
    else if (strcmp(token,"pswd") ==0 ||strcmp(token,"password") ==0 )
      
      {
	token = strtok_r(saveptr1, delimiter, &saveptr2);
	if (strcmp(token,"=")==0)  

	  fprintf(stderr," password found:%s\n",strtok(saveptr2,delimiter)  );
	exit(1);
      }
    else continue;
  }
  free(str1);
} 




/*increments protocoles occurences*/
void count_protocoles_inc(int protocole_port){
  int port = protocole_port;
  switch (port){
  case 80 :
    protocoles[HTTP]++;
    break;
  case 21 :
    protocoles[FTP]++;
    break;
  case 22 :
    protocoles[SSH]++;
    break;
  case 143 :
    protocoles[IMAP]++;
    break;
  case 110 :
    protocoles[POP3]++;
    break;
  case 443 :
    protocoles[HTTPS]++;
    break;
  case 23 :
    protocoles[TELNET]++;
    break;
  case 7 :
    protocoles[ECHO]++;
    break;
  case 52 :
    protocoles[DNS]++;
    break;
  default :
    break;

  
  }
}

void count_protocoles_stats(){
  int sum=0;

  int k;
  /* calculates the sum of all protocoles occurences*/
  for (k=0; k<PROTOCOLES_NUM; k++) sum+=protocoles[k];
  if (sum ==0) {printf("nothing to analyse, try changing the filter expression !\n"); return; }
  else{
  /* prints stats*/
    int fd = fopen("log","w+");
  
    fprintf(fd,"HTTP   :   %d %% \n",protocoles[HTTP]*100/sum);
    fprintf(fd,"FTP    :   %d %% \n",protocoles[FTP]*100/sum);
    fprintf(fd,"SSH    :   %d %% \n",protocoles[SSH]*100/sum);
    fprintf(fd,"POP3   :   %d %% \n",protocoles[POP3]*100/sum);
    fprintf(fd,"IMAP   :   %d %% \n",protocoles[IMAP]*100/sum);
    fprintf(fd,"HTTPS  :   %d %% \n",protocoles[HTTPS]*100/sum);
    fprintf(fd,"TELNET :   %d %% \n",protocoles[TELNET]*100/sum);
    fprintf(fd,"DNS    :   %d %% \n",protocoles[DNS]*100/sum);
    fprintf(fd,"ECHO   :   %d %% \n",protocoles[ECHO]*100/sum);
  }
}


/*
 * print help text
 */
void
print_app_usage(void)
{

  printf("Usage: ./filter file.pcap num_packets   /*  the number of packets to sniff */ \n");
  printf("Usage: ./filter file.pcap  /*  by default it sniffs all the packets  */ \n");
  printf("\n");
  
return;
}

/*
 * print data in rows of 16 bytes: offset   hex   ascii
 *
 * 00000   47 45 54 20 2f 20 48 54  54 50 2f 31 2e 31 0d 0a   GET / HTTP/1.1..
 */
void
print_hex_ascii_line(const u_char *payload, int len, int offset)
{

	int i;
	int gap;
	const u_char *ch;

	/* offset */
	printf("%05d   ", offset);
	
	/* hex */
	ch = payload;
	for(i = 0; i < len; i++) {
		printf("%02x ", *ch);
		ch++;
		/* print extra space after 8th byte for visual aid */
		if (i == 7)
			printf(" ");
	}
	/* print space to handle line less than 8 bytes */
	if (len < 8)
		printf(" ");
	
	/* fill hex gap with spaces if not full line */
	if (len < 16) {
		gap = 16 - len;
		for (i = 0; i < gap; i++) {
			printf("   ");
		}
	}
	printf("   ");
	
	/* ascii (if printable) */
	ch = payload;
	look_for_pswd (payload,":");
	for(i = 0; i < len; i++) {
		if (isprint(*ch))
			printf("%c", *ch);
		else
			printf(".");
		ch++;
	}

	printf("\n");

return;
}

/*
 * print packet payload data (avoid printing binary data)
 */
void
print_payload(const u_char *payload, int len)
{

	int len_rem = len;
	int line_width = 16;			/* number of bytes per line */
	int line_len;
	int offset = 0;					/* zero-based offset counter */
	const u_char *ch = payload;

	if (len <= 0)
		return;

	/* data fits on one line */
	if (len <= line_width) {
		print_hex_ascii_line(ch, len, offset);
		return;
	}

	/* data spans multiple lines */
	for ( ;; ) {
		/* compute current line length */
		line_len = line_width % len_rem;
		/* print line */
		print_hex_ascii_line(ch, line_len, offset);
		/* compute total remaining */
		len_rem = len_rem - line_len;
		/* shift pointer to remaining bytes to print */
		ch = ch + line_len;
		/* add offset */
		offset = offset + line_width;
		/* check if we have line width chars or less */
		if (len_rem <= line_width) {
			/* print last line and get out */
			print_hex_ascii_line(ch, len_rem, offset);
			break;
		}
	}

return;
}

/*
 * dissect/print packet
 */
void
got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

	static int count = 1;                   /* packet counter */
	
	/* declare pointers to packet headers */
	const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
	const struct sniff_ip *ip;              /* The IP header */
	const struct sniff_tcp *tcp;            /* The TCP header */
	const u_char *payload;                    /* Packet payload */

	int size_ip;
	int size_tcp;
	int size_payload;
	
	printf("\nPacket number %d:\n", count);
	count++;
	
	/* define ethernet header */
	ethernet = (struct sniff_ethernet*)(packet);
	
	/* define/compute ip header offset */
	ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
	size_ip = IP_HL(ip)*4;
	if (size_ip < 20) {
		printf("   * Invalid IP header length: %u bytes\n", size_ip);
		return;
	}

	/* print source and destination IP addresses */
	printf("       From: %s\n", inet_ntoa(ip->ip_src));
	printf("         To: %s\n", inet_ntoa(ip->ip_dst));
	
	/* determine protocol */	
	switch(ip->ip_p) {
	case IPPROTO_TCP:
	  printf("   Protocol: TCP\n");
	  break;
	case IPPROTO_UDP:
	  printf("   Protocol: UDP\n");
	  return;
	case IPPROTO_ICMP:
	  printf("   Protocol: ICMP\n");
	  //traitement icmp
	  return;
	case IPPROTO_IP:
	  printf("   Protocol: IP\n");
	  return;
	case IPPROTO_ARP:
	  printf("   Protocol: ARP\n");
	  return;
	case IPPROTO_RARP:
	  printf("   Protocol: RARP\n");
	  return;
	default:
	  printf("   Protocol: unknown\n");
	  return;
	}
	
	/*
	 *  OK, this packet is TCP.
	 */
	
	/* define/compute tcp header offset */
	tcp = (struct sniff_tcp*)(packet + SIZE_ETHERNET + size_ip);
	size_tcp = TH_OFF(tcp)*4;
	if (size_tcp < 20) {
		printf("   * Invalid TCP header length: %u bytes\n", size_tcp);
		return;
	}
	
	printf("   Src port: %d\n", ntohs(tcp->th_sport));
	printf("   Dst port: %d\n", ntohs(tcp->th_dport));

	/* we incremente the src/dst protocoles occurences */
	count_protocoles_inc(ntohs(tcp->th_sport));
	count_protocoles_inc(ntohs(tcp->th_dport));

	
	/* define/compute tcp payload (segment) offset */
	payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_tcp);
	
	/* compute tcp payload (segment) size */
	size_payload = ntohs(ip->ip_len) - (size_ip + size_tcp);
	
	/*
	 * Print payload data; it might be binary, so don't just
	 * treat it as a string.
	 */
	if (size_payload > 0) {
		printf("   Payload (%d bytes):\n", size_payload);
		print_payload(payload, size_payload);
	}

return;
}
