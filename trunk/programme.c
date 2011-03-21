/*  Packet sniffer using libpcap library */
#include<pcap.h> 
#include<stdio.h>
#include<stdlib.h> 
#include<assert.h>
#include<net/ethernet.h> 
#include<netinet/ip_icmp.h> //Provides declarations for icmp header 
#include<netinet/udp.h> //Provides declarations for udp header 
#include<netinet/tcp.h> //Provides declarations for tcp header 
#include<netinet/ip.h> //Provides declarations for ip header   
#include <glib.h>

/*****************
 *Global variables
 *****************/
FILE *logfile;

struct sockaddr_in source,dest;
//pour le format pcap: file header
typedef struct pcap_hdr_s {
        guint32 magic_number;   /* magic number */
        guint16 version_major;  /* major version number */
        guint16 version_minor;  /* minor version number */
        gint32  thiszone;       /* GMT to local correction */
        guint32 sigfigs;        /* accuracy of timestamps */
        guint32 snaplen;        /* max length of captured packets, in octets */
        guint32 network;        /* data link type */
} pcap_hdr_t;

//packet header
typedef struct pcaprec_hdr_s {
        guint32 ts_sec;         /* timestamp seconds */
        guint32 ts_usec;        /* timestamp microseconds */
        guint32 incl_len;       /* number of octets of packet saved in file */
        guint32 orig_len;       /* actual length of packet */
} pcaprec_hdr_t;

void print_pcap_format(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

/****************
 *Main function
 ****************/
int main() {
  pcap_if_t alldevsp[100];       /*all found device*/
  pcap_if_t*device;              /*the device to sniff on*/
  pcap_t *handle;                /*Handle of the device that shall be sniffed*/
  char errbuf[PCAP_ERRBUF_SIZE]; /*Error string*/
  char*devname;                  /*device name */
  char *devs[20];        
  int count = 0 , n;             /*interger used for boucles*/
  
  //First get the list of available devices
  printf("Finding available devices ...\t");
  if(pcap_findalldevs(&alldevsp, errbuf))  {
    printf("Error finding devices : %s\n" , errbuf);
    exit(1);
  }
  printf("Done\n");

  //Print the available devices
  printf("Available Devices are :\n");
  device = alldevsp;
  while(device != NULL)  {
    devs[count] = malloc(sizeof(device->name));
    *(devs[count]) = device->name;
    printf("%d. %s - %s\n", count++ , device->name , device->description);
    device = device->next;
  }

  //Ask user which device to sniff
  printf("Enter the number of the device you want to sniff : \n");
  scanf("%d" , &n);
  devname = devs[n];

  //Open the device for sniffing
  printf("Opening device for sniffing ...\t");
  handle = pcap_open_live("eth1" , BUFSIZ , 1 , 1000 , errbuf);
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device eth0 : %s\n" , errbuf);
    exit(1);
  }  
  printf("Done\n");

  //open the file to write in
  logfile=fopen("log.txt","w");
  if(logfile==NULL)
    printf("Unable to create file.\n");

  //Put the device in sniff loop  
  pcap_loop(handle , -1 , print_pcap_format , NULL);
  return 0; 
}

//pcap_pkthdr contient tte les info concernant un packet capté
void print_pcap_format(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){
  pcaprec_hdr_t * filePcap = (pcaprec_hdr_t*)malloc(sizeof(pcaprec_hdr_t));
  assert(filePcap);
  fprintf(logfile,"la max d'octets captés pour chaque packets %d\n",filePcap->incl_len);




}
