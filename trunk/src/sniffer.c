
/* Expression			Description
 * ----------			-----------
 * ip					Capture all IP packets.
 * tcp					Capture only TCP packets.
 * tcp port 80			Capture only TCP packets with a port equal to 80.
 * ip host 10.1.2.3		Capture all IP packets to or from host 10.1.2.3.
 *
 ****************************************************************************
 *
 */
#include "process_server.h"
#include <time.h>

/* filter expression, nothing to compile */
char filter_exp[] = " ip or tcp or udp or icmp or arp or rarp"; 



void* start_sniffing(void * num)
{


  int  num_packet = *((int *)num);
  fprintf(stderr,"sniffer : num_packet %d\n",num_packet);
  char *devs[20];            /* a tab that contains all devices names*/
  pcap_if_t *alldevsp[100];       /*all found devices */
  pcap_if_t* device;              /*the device to sniff on*/
  char errbuf[PCAP_ERRBUF_SIZE];		/* error buffer */
  pcap_t *handle;				/* packet capture handle */
  char * dev = NULL;                              /* capture device name */
  
  int count = 0 , n;             /*interger used for boucles*/  

  struct bpf_program fp;			/* compiled filter program (expression) */
  bpf_u_int32 mask;			/* subnet mask */
  bpf_u_int32 net;			/* ip */
  
/*%%%%%%First get the list of available devices%%%%%%*/
  printf("Finding available devices ...\t");
  if(pcap_findalldevs(alldevsp, errbuf))  {
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
  dev = devs[n];

  
  /* get network number and mask associated with capture device */
  if (pcap_lookupnet(dev, &net, &mask, errbuf) == -1) {
    fprintf(stderr, "Couldn't get netmask for device %s: %s\n",dev, errbuf);
    net = 0;
    mask = 0;
  }
  
  /* print capture info */
  printf("Device: %s\n", dev);
  
  /* open capture device */ 
  handle = pcap_open_live(dev, SNAP_LEN, 1, 1000, errbuf); //promiscuous mode
  if (handle == NULL) {
    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    exit(EXIT_FAILURE);
  }
  
  /* make sure we're capturing on an Ethernet device [2] */
  if (pcap_datalink(handle) != DLT_EN10MB) {
    fprintf(stderr, "%s is not an Ethernet\n",dev);
    exit(EXIT_FAILURE);
  }
  
  /* compile the filter expression */
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1) {
    fprintf(stderr, "Couldn't parse filter %s: %s\n",
	    filter_exp, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }
  
  /* apply the compiled filter */
  if (pcap_setfilter(handle, &fp) == -1) {
    fprintf(stderr, "Couldn't install filter %s: %s\n",
	    filter_exp, pcap_geterr(handle));
    exit(EXIT_FAILURE);
  }
  
  /* now we can set our callback function */
  pcap_loop(handle, 10, got_packet, NULL);
  
  /* const u_char * packet; */
  /* struct pcap_pkthdr header; */
  /* do   */
  /*   pcap_dispatch(handle, num_packet, got_packet, NULL); */
  /* while(packet=pcap_next(handle,&header));   */

  /* cleanup */
  pcap_freecode(&fp);
  pcap_close(handle);
  
  printf("\nCapture complete.\n");
  
  return NULL;
}
