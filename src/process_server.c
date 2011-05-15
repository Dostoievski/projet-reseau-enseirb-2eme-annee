/**
 * \file process_server.c
 * \brief Programme vérifiant qu'un client est inscrit dans un tableau de protocole(ip par exemple) et lui envoye le packet correspondant à ce protocole.
 * \author RedNetwork
 * \version 0.1
 * \date 16 Mai 2011
 */
#include "process_server.h"
#include "requests.h"

/**
 * \fn int send_packet_to_client(u_char * packet, int packet_size, int * tab)
 * \brief Fonction qui envoye un packet à un client inscrit dans un tableau de protocole.
 *
 * \param packet : packet à envoyer aux clients.
 * \param packet_size : taille du packet.
 * \param tab : tableau de protocole dans lequel les client sont inscrits.
 * \return 1 si tout va bien.
 */
int send_packet_to_client(u_char * packet, int packet_size, int * tab){
  int i;
  int size = packet_size;
  unsigned char * to_send = packet;
  struct answer answer_sniff;
  
  //les tableaux de protocoles tab font une taille de MAXCLIENTS 
  for(i=0;i<MAXCLIENTS;i++){
    /*si un clientfd est inscrit dans la table*/
    if (tab[i] !=0){
      int clientfd = tab[i];
      fprintf(stderr,"envoi d'un packet au client n°%d\n",tab[i]);
      //on envoie un ACK au client
      answer_sniff.ack=ANSWER_OK;
      answer_sniff.nbbytes=size;
      if (write(clientfd,&answer_sniff,sizeof(answer_sniff)) == -1){   
	printf("Erreur lors de l\'envoi d\'une réponse: sniff"); 
	exit(EXIT_FAILURE);       
      }
      //copie des données du packet vers le client
      if (write(clientfd,to_send,size) == -1){   
	fprintf(stderr,"Erreur lors de l\'envoi du packet: sniff"); 
	exit(EXIT_FAILURE);      
      }
    }
  }
  return 1;
}


/*
 * pcap call back function
 */

/**
 * \fn got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
 * \brief Fonction appelée par pcap_loop, elle détermine le protocole du packet capté et l'envoie aux clients correspondants.
 *
 * \param args : nom du packet capté.
 * \param header : l'entete du packet.
 * \param packet : packet capté.
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
  u_char * packet_to_send = packet;
  static int count = 1;                   /* packet counter */
  
  /* declare pointers to packet headers */
  const struct sniff_ethernet *ethernet;  /* The ethernet header [1] */
  const struct sniff_ip *ip;              /* The IP header */
  const struct sniff_tcp *tcp;            /* The TCP header */
  const struct sniff_udp *udp;
  const struct sniff_icmp *icmp;
  int size_ip;
  count++;
  
  /* define ethernet header */
  ethernet = (struct sniff_ethernet*)(packet);
  
  /* define/compute ip header offset */
  ip = (struct sniff_ip*)(packet + SIZE_ETHERNET);
  size_ip = IP_HL(ip)*4;
  if (size_ip < 20) {
    printf(" * Invalid IP header length: %u bytes\n", size_ip);
    return;
  }

  /* determine protocol */	
  switch(ip->ip_p) {
  case IPPROTO_TCP:
    send_packet_to_client(packet_to_send ,SIZE_ETHERNET+ip->ip_len,TCP);
    break;
  case IPPROTO_UDP:
    // udp=(struct sniff_udp*)(packet+SIZE_ETHERNET+size_ip);	
    send_packet_to_client(packet_to_send,SIZE_ETHERNET+ip->ip_len,UDP);
    return;
  case IPPROTO_ICMP:
    //icmp=(struct sniff_icmp*)(packet+SIZE_ETHERNET+size_ip);	
    send_packet_to_client(packet_to_send ,SIZE_ETHERNET+ip->ip_len,ICMP);
    return;
  case IPPROTO_IP:
    send_packet_to_client(packet_to_send ,SIZE_ETHERNET+ip->ip_len,IP);
    return;
  case IPPROTO_ARP:
    send_packet_to_client(packet_to_send ,SIZE_ETHERNET+ip->ip_len,ARP);
    return;
  case IPPROTO_RARP:
    send_packet_to_client(packet_to_send ,SIZE_ETHERNET+ip->ip_len,RARP);
    return;
  default:
    return;
  }
  return;
}

/**
 * \fn check_protocole(char * protocole)
 * \brief Fonction qui verifie que le protocole que désire le client est bien pris en charge.
 *
 * \param protocole : protocole désiré par le client.
 */
void check_protocole(char * protocole){
  char * name = protocole;
  /*si le protocole n'est pas ip ou tcp ou udp ou icmp, il y a erreur*/ 
  if ((strcmp(name,"ip") != 0) && (strcmp(name,"udp") != 0) && (strcmp(name,"tcp") != 0)&& (strcmp(name,"icmp") != 0) && (strcmp(name,"arp") != 0) && (strcmp(name,"rarp") != 0) ){
    
    fprintf(stderr,"le serveur ne prend pas en charge ce protocole\n");
    exit(EXIT_FAILURE);
  }
}
