/*****************************************
 *  Packet sniffer using libpcap library *
 *****************************************/
#include<pcap.h> 
#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<net/ethernet.h> 
#include<netinet/ip_icmp.h> //Provides declarations for icmp header 
#include<netinet/udp.h> //Provides declarations for udp header 
#include<netinet/tcp.h> //Provides declarations for tcp header 
#include<netinet/ip.h> //Provides declarations for ip header   
#include <sys/time.h>
#include <unistd.h>

#include "programme.h"
#include "createFiles.h"
#include "time.h"
/*****************
 *Global variables
 *****************/
FILE *logfile;

/****************
 *Main function
 ****************/


int main() {

  pcap_if_t alldevsp[100];       /*all found device*/
  pcap_if_t*device;              /*the device to sniff on*/
  pcap_dumper_t *dumpdesc;       /*structure represanting the opened file */
  pcap_t *handle;                /*Handle of the device that shall be sniffed*/  
  char errbuf[PCAP_ERRBUF_SIZE]; /*Error string*/
  char*devname;                  /*device name */
  char *devs[20];        
  int count = 0 , i=0, n;             /*interger used for boucles*/  
 
  /*%%%%%%First get the list of available devices%%%%%%*/
  printf("Finding available devices ...\t");
  if(pcap_findalldevs(&alldevsp, errbuf))  {
    printf("Error finding devices : %s\n" , errbuf);
    exit(1);
  }
  printf("Done\n");

  /*%%%%%%Print the available devices%%%%%%*/
  printf("Available Devices are :\n");
  device = alldevsp;
  while(device != NULL)  {
    devs[count] = malloc(sizeof(device->name));
    *(devs[count]) = device->name;
    printf("%d. %s - %s\n", count++ , device->name , device->description);
    device = device->next;
  }

  /*%%%%%%Ask user which device to sniff%%%%%%*/
  printf("Enter the number of the device you want to sniff : \n");
  scanf("%d" , &n);
  devname = devs[n];

  /*%%%%%%Open the device for sniffing%%%%%%*/
  printf("Opening device for sniffing ...\t");
  handle = pcap_open_live("eth0" , BUFSIZ , 1 , 1000 , errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device eth0 : %s\n" , errbuf);
    exit(1);
  }  
  printf("Done\n");

  /*%%%%%%Create a processus that creates a file 
    every 1min and writes 
    in it the captured packets%%%%%%%%  */

  pid_t pid = createProcessus();

  printf("valeur de fork %d \n",pid);

  switch(pid){
  case -1:
    printf("Eror can't open the child processus\n");
    exit(-1);
    break;
  case 1:
    printf("processeur pere\n");
    break;
  case 0:
    printf("Files creation \n");
    double t1, t2;
    while(1){
      if(one_minute_past(t1,t2) == 0){
	i++;
	FILE* f = createFile(i);
	pcap_dumper_t* dumpdesc = createPcapFile(handle, f);
	pcap_loop(handle ,10 ,pcap_dump ,(u_char*)dumpdesc);
	printf("sniffing done\n");
	pcap_dump_close(dumpdesc);
      }
      t2=t1;
     
    }
    pcap_close(handle);

    break;
  default:
    printf("erreur Fork\n");
  }
  return 0;
}

















