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
#include <glib.h>
#include "programme.h"

/*****************
 *Global variables
 *****************/
FILE *logfile;

/****************
 *Main function
 ****************/
float get_time() {
  struct timeval tv;
  gettimeofday(&tv,NULL);
  return 1000*tv.tv_sec + tv.tv_usec/1000.0;
}

int main() {

  pcap_if_t alldevsp[100];       /*all found device*/
  pcap_if_t*device;              /*the device to sniff on*/
  pcap_dumper_t *dumpdesc;       /*structure represanting the opened file */
  pcap_t *handle;                /*Handle of the device that shall be sniffed*/  
  char errbuf[PCAP_ERRBUF_SIZE]; /*Error string*/
  char*devname;                  /*device name */
  char *devs[20];        
  int count = 0 , n;             /*interger used for boucles*/
  
 
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
  handle = pcap_open_live("eth1" , BUFSIZ , 1 , 1000 , errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device eth0 : %s\n" , errbuf);
    exit(1);
  }  
  printf("Done\n");

  /*%%%%%%open the file to write in%%%%%%*/
  logfile=fopen("log.pcap","wb");
  if(logfile==NULL)
    printf("Unable to create file.\n");
  
  dumpdesc = pcap_dump_fopen(handle, logfile);
  if(dumpdesc == NULL){
    printf("ERROR dump\n");
    exit(1);
  }
 
  pcap_hdr_t * filePcap = (pcap_hdr_t*)malloc(sizeof(pcap_hdr_t));
  if(filePcap == NULL){
    printf("Error \n");
    exit(-1);
  }
  //filePcap->snaplen = (guint32)header->len;
  printf("*******************************Global Header****************************\n");
  //fwrite(filePcap,sizeof(filePcap),1,logfile);
  
  /*%%%%%%Put the device in sniff loop: capter 10 paquets
    and write them in lofile %%%%*/ 
  while(1){  
    pcap_loop(handle , 10 ,(pcap_handler)pcap_dump ,(u_char*)dumpdesc);
    printf("dump 10 packet done\n");
  }
  pcap_dump_close(dumpdesc);
  
  pcap_close(handle);
  
  return 0;
  
}

//pcap_pkthdr contient tte les info concernant un packet capté
/*
struct pcap_pkthdr {
                struct timeval ts; //time stamp 
		bpf_u_int32 caplen; // length of portion present 
		bpf_u_int32 len; // length this packet (off wire) 
	}

args corresponds to the last argument of pcap_loop()

A packet contains many attributes, so as you can imagine, 
it is not really a string, but actually a collection of structures  
*/















