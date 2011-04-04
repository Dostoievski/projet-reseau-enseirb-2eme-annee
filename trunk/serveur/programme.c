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
  pcap_if_t* device;              /*the device to sniff on*/
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
    devs[count] = device->name;
    printf("%d. %s - %s\n", count++ , device->name , device->description);
    device = device->next;
  }

  /*%%%%%%Ask user which device to sniff%%%%%%*/
  printf("Enter the number of the device you want to sniff : \n");
  scanf("%d" , &n);
  devname = devs[n];

  /*%%%%%%Open the device for sniffing%%%%%%*/
  printf("Opening device for sniffing ...\t");
  handle = pcap_open_live(devname , BUFSIZ , 1 , 1000 , errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s : %s\n" , devname, errbuf);
    exit(1);
  }  
  printf("Done\n");

  /*%%%%%%Create a processus that creates a file 
    every 1min and writes 
    in it the captured packets%%%%%%%%  */

  pid_t pid = fork();//createProcessus();

  printf("valeur de fork %d \n",pid);

  switch(pid){
  case -1:
    printf("Eror can't open the child processus\n");
    exit(-1);
    break;
  case 0:
    printf("Files creation \n");
    double t1 = get_time(), t2;
    FILE* f = createFile(i);
    pcap_dumper_t* dumpdesc = createPcapFile(handle, f);
    while(1){
      pcap_loop(handle ,1 ,pcap_dump ,(u_char*)dumpdesc);
      t2 = get_time();
      if(t2 - t1 > 60.0){
	t1 = t2;
	i++;
	printf("done\n");
	f = createFile(i);
	dumpdesc = createPcapFile(handle, f);
	pcap_dump_close(dumpdesc);
      }
    }
    pcap_close(handle);
    break;
  default :
    waitpid(pid, NULL,0);
    printf("Capture terminée!!\n");
  } 
  return 0;
}

















