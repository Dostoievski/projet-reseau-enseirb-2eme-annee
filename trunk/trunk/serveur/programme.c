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
#include <pthread.h>
#include "programme.h"
#include "createFiles.h"
#include "time.h"
/*****************
 *Global variables
 *****************/
FILE *logfile;
pcap_dumper_t* dumpdesc;
FILE* dumping_file;
pcap_t*handle;/*Handle of the device that shall be sniffed*/
int FERME = 0;

void * create_dumping_file(){
  int i = 0;
  while(1){
    i++;
    printf("3-ICI\n");
    sleep(60);
    fprintf(stderr,"4-ICI");
    fprintf(stdout, "done\n");
    if(FERME) 
      pcap_dump_close(dumpdesc);
    fclose(dumping_file);
    dumping_file = createFile(i);
    dumpdesc = createPcapFile(handle, dumping_file);
  }
}

/****************
 *Main function
 ****************/


int main() {

  pcap_if_t alldevsp[100];       /*all found device*/
  pcap_if_t* device;              /*the device to sniff on*/
  pcap_dumper_t *dumpdesc;       /*structure represanting the opened file */
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

  pid_t pid = 0;

  printf("valeur de fork %d \n",pid);

  switch(pid){
  case -1:
    printf("Eror can't open the child processus\n");
    exit(-1);
    break;
  case 0:
    fprintf(stdout,"Files creation \n");
    dumping_file = createFile(0);
    dumpdesc = createPcapFile(handle, dumping_file);
    printf("1-ICI\n");
    pthread_t th;
    pthread_create(&th,NULL,create_dumping_file,NULL);
    while(1){
      //t2 = get_time();
      //printf("time = %g\n", t2-t1);
      FERME = 0;
      pcap_loop(handle ,1 ,pcap_dump ,(u_char*)dumpdesc);
      FERME = 1;
      fprintf(stderr,"2-ICI");
      //printf("time = %g\n", t2-t1);
      //if(t2 - t1 > 5.0){
      //t1 = t2;
      //i++;
      //fprintf(stdout, "done\n");
      //f = createFile(i);
      //
    }
    
    pcap_close(handle);
    break;
  default :
    waitpid(pid, NULL,0);
    printf("Capture terminée!!\n");
  } 
  return 0;
}

















